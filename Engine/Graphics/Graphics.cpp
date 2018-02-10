// Include Files
//==============

#include "Graphics.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cSamplerState.h"
#include "sContext.h"
#include "cEffect.h"
#include "cSprite.h"
#include "cMesh.h"
#include "cView.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>

#include <algorithm>
// Static Data Initialization
//===========================

namespace
{
	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perFrame(eae6320::Graphics::ConstantBufferTypes::PerFrame);
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perDrawCall(eae6320::Graphics::ConstantBufferTypes::PerDrawCall);

	// In our class we will only have a single sampler state
	eae6320::Graphics::cSamplerState s_samplerState;

	// Submission Data
	//----------------
	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sPerFrame constantData_perFrame;
		std::vector<eae6320::Graphics::ConstantBufferFormats::sPerDrawCall> constantData_perDrawCallContainer;

		const float *BackGroundColor;

		std::vector<std::pair<eae6320::Graphics::cEffect *, eae6320::Graphics::cSprite *>> TempSpritePairContainer;
		std::vector<eae6320::Graphics::cTexture *> TempSpriteTextureContainer;
		std::vector<std::pair<eae6320::Graphics::cEffect *, eae6320::Graphics::cMesh *>> TempOpaqueMeshPairContainer;
		std::vector<eae6320::Graphics::cTexture *> TempOpaqueMeshTextureContainer;
		std::vector<std::pair<eae6320::Graphics::cEffect *, eae6320::Graphics::cMesh *>> TempTransMeshPairContainer;
		std::vector<eae6320::Graphics::cTexture *> TempTransMeshTextureContainer;

		eae6320::Graphics::EClearState ClearState;						// used in graphics clean up

		// when we submit data, we must use data from application to init data in our container (in order to cut the relation between each other)													
		void SetContainer(const std::vector<eae6320::Graphics::SpriteDataGroup> &i_spriteDataGroups, const std::vector<eae6320::Graphics::MeshDataGroup> &i_opaqueMeshDataGroups, const std::vector<eae6320::Graphics::MeshDataGroup> &i_transMeshDataGroups);
		void ClearContainer();
	};

	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be getting populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated, 
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	sDataRequiredToRenderAFrame *s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	sDataRequiredToRenderAFrame *s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;

	// view
	eae6320::Graphics::cView s_viewInstance;

}


// Interface
//==========

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perFrame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_perFrame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

void eae6320::Graphics::SubmitCameraData(const Math::cMatrix_transformation &i_cameraProjectedMatrix, const Math::sVector &i_cameraPos, const eae6320::Math::cQuaternion &i_cameraQuat)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perFrame.g_transform_cameraToProjected = i_cameraProjectedMatrix;
	constantData_perFrame.g_transform_worldToCamera = Math::cMatrix_transformation::CreateWorldToCameraTransform(i_cameraQuat, i_cameraPos);
}



void eae6320::Graphics::SubmitRenderData(const float* i_backGroundColor, const std::vector<SpriteDataGroup> &i_spriteDataGroups,
	const std::vector<MeshDataGroup> &i_opaqueMeshDataGroups, const std::vector<MeshDataGroup> &i_transMeshDataGroups)
	//void eae6320::Graphics::SubmitRenderData(const float* i_backGroundColor, std::vector<std::pair<cEffect *, cSprite *>> &i_spritePairContainer, std::vector<cTexture *> i_spriteTextureContainer, 
	//											std::vector<std::pair<cEffect *, cMesh *>> &i_meshPairContainer, std::vector<std::pair<Math::sVector, Math::cQuaternion>> &i_meshData, std::vector<cTexture *> i_meshTextureContainer)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	// camera data already get
	auto & constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	auto & constantData_perDrawCallContainer = s_dataBeingSubmittedByApplicationThread->constantData_perDrawCallContainer;
	size_t opaqueMeshCount = i_opaqueMeshDataGroups.size(), transMeshCount = i_transMeshDataGroups.size();
	constantData_perDrawCallContainer.resize(opaqueMeshCount + transMeshCount);
	for (unsigned int i = 0; i < opaqueMeshCount; ++i)
	{
		constantData_perDrawCallContainer[i].g_transform_localToProjected = constantData_perFrame.g_transform_cameraToProjected *
			Math::cMatrix_transformation::ConcatenateAffine(constantData_perFrame.g_transform_worldToCamera, Math::cMatrix_transformation(i_opaqueMeshDataGroups[i].Rotation, i_opaqueMeshDataGroups[i].Position));
	}

	// sort translucent mesh back to front
	std::vector<MeshDataGroup> tempTransMeshDataGroup = i_transMeshDataGroups;
	for (unsigned int i = 0; i < tempTransMeshDataGroup.size(); ++i)
	{
		tempTransMeshDataGroup[i].ZDepth = (constantData_perFrame.g_transform_worldToCamera * Math::cMatrix_transformation(i_transMeshDataGroups[i].Rotation, i_transMeshDataGroups[i].Position).GetTranslation()).z;
	}
	sort(tempTransMeshDataGroup.begin(), tempTransMeshDataGroup.end(), MeshDataGroup::CompareRenderOrder);
	for (unsigned int i = 0; i < transMeshCount; ++i)
	{
		constantData_perDrawCallContainer[i + opaqueMeshCount].g_transform_localToProjected = constantData_perFrame.g_transform_cameraToProjected *
			Math::cMatrix_transformation::ConcatenateAffine(constantData_perFrame.g_transform_worldToCamera, Math::cMatrix_transformation(tempTransMeshDataGroup[i].Rotation, tempTransMeshDataGroup[i].Position));
	}

	s_dataBeingSubmittedByApplicationThread->BackGroundColor = i_backGroundColor;
	s_dataBeingSubmittedByApplicationThread->SetContainer(i_spriteDataGroups, i_opaqueMeshDataGroups, tempTransMeshDataGroup);
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}


// Render
//-------

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		const auto result = Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread);
		if (result)
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			const auto result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal();
			if (!result)
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	// Update the per-frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_perFrame = s_dataBeingRenderedByRenderThread->constantData_perFrame;
		s_constantBuffer_perFrame.Update(&constantData_perFrame);
	}

	s_viewInstance.ClearView(s_dataBeingRenderedByRenderThread->BackGroundColor[0], s_dataBeingRenderedByRenderThread->BackGroundColor[1], s_dataBeingRenderedByRenderThread->BackGroundColor[2], s_dataBeingRenderedByRenderThread->BackGroundColor[3]);
	s_viewInstance.ClearDepth();

	// Bind and Draw Opaque Mesh
	unsigned int opaqueMeshPairs = static_cast<unsigned int>(s_dataBeingRenderedByRenderThread->TempOpaqueMeshPairContainer.size());
	for (unsigned int i = 0; i < opaqueMeshPairs; ++i)
	{
		// Update the per-drawCall constant buffer
		{
			// Copy the data from the system memory that the application owns to GPU memory
			auto& constantData_perDrawCall = s_dataBeingRenderedByRenderThread->constantData_perDrawCallContainer[i];
			s_constantBuffer_perDrawCall.Update(&constantData_perDrawCall);
		}

		s_dataBeingRenderedByRenderThread->TempOpaqueMeshPairContainer[i].first->Bind();
		s_dataBeingRenderedByRenderThread->TempOpaqueMeshTextureContainer[i]->Bind(0);
		s_dataBeingRenderedByRenderThread->TempOpaqueMeshPairContainer[i].second->Draw();
	}

	// Bind and Draw Translucent Mesh
	unsigned int transMeshPairs = static_cast<unsigned int>(s_dataBeingRenderedByRenderThread->TempTransMeshPairContainer.size());
	for (unsigned int i = 0; i < transMeshPairs; ++i)
	{
		{
			auto& constantData_perDrawCall = s_dataBeingRenderedByRenderThread->constantData_perDrawCallContainer[i + opaqueMeshPairs];
			s_constantBuffer_perDrawCall.Update(&constantData_perDrawCall);
		}

		s_dataBeingRenderedByRenderThread->TempTransMeshPairContainer[i].first->Bind();
		s_dataBeingRenderedByRenderThread->TempTransMeshTextureContainer[i]->Bind(0);
		s_dataBeingRenderedByRenderThread->TempTransMeshPairContainer[i].second->Draw();
	}

	// Bind and Draw sprite
	unsigned int spritePairs = static_cast<unsigned int>(s_dataBeingRenderedByRenderThread->TempSpritePairContainer.size());
	for (unsigned int i = 0; i < spritePairs; ++i)
	{
		s_dataBeingRenderedByRenderThread->TempSpritePairContainer[i].first->Bind();
		s_dataBeingRenderedByRenderThread->TempSpriteTextureContainer[i]->Bind(0);
		s_dataBeingRenderedByRenderThread->TempSpritePairContainer[i].second->Draw();
	}

	s_viewInstance.SwapBuffer();

	// Once everything has been drawn the data that was submitted for this frame
	// should be cleaned up and cleared.
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		s_dataBeingRenderedByRenderThread->ClearContainer();
	}
}



// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	// Initialize the asset managers
	{
		if (!(result = cShader::s_manager.Initialize()))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}
	// Initialize the asset managers
	{
		if (!(result = cTexture::s_manager.Initialize()))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_perFrame.Initialize())
		{
			// There is only a single per-frame constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perFrame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment);
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if (result = s_constantBuffer_perDrawCall.Initialize())
		{
			// There is only a single per-frame constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perDrawCall.Bind(
				ShaderTypes::Vertex);
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if (result = s_samplerState.Initialize())
		{
			// There is only a single sampler state that is re-used
			// and so it can be bound at initialization time and never unbound
			s_samplerState.Bind();
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}
	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	// Initialize the views
	{
		if (!(result = s_viewInstance.Initialize(i_initializationParameters)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:

	return result;
}

// this should be done before cleanUp
void eae6320::Graphics::ClearBuffers()
{
	// clear 2 buffer
	s_dataBeingRenderedByRenderThread->ClearContainer();
	s_dataBeingSubmittedByApplicationThread->ClearContainer();
}


eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	s_viewInstance.CleanUp();

	{
		const auto localResult = s_constantBuffer_perFrame.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = s_constantBuffer_perDrawCall.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = s_samplerState.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	// clean up textures
	{
		const auto localResult = cTexture::s_manager.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	// clean up shaders
	{
		const auto localResult = cShader::s_manager.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	// clean up context
	{
		const auto localResult = sContext::g_context.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	return result;
}


void sDataRequiredToRenderAFrame::SetContainer(const std::vector<eae6320::Graphics::SpriteDataGroup> &i_spriteDataGroups, const std::vector<eae6320::Graphics::MeshDataGroup> &i_opaqueMeshDataGroups, const std::vector<eae6320::Graphics::MeshDataGroup> &i_transMeshDataGroups)
//void sDataRequiredToRenderAFrame::SetContainer(std::vector<std::pair<eae6320::Graphics::cEffect *, eae6320::Graphics::cSprite *>> &i_spritePairs, std::vector<eae6320::Graphics::cTexture *> &i_spriteTextures, std::vector<std::pair<eae6320::Graphics::cEffect *, eae6320::Graphics::cMesh *>> &i_meshPairs, std::vector<eae6320::Graphics::cTexture *> &i_meshTextures)
{
	EAE6320_ASSERT(ClearState == eae6320::Graphics::EClearState::Clear);

	unsigned int pairs = static_cast<unsigned int>(i_spriteDataGroups.size());
	TempSpritePairContainer.resize(pairs);
	TempSpriteTextureContainer.resize(pairs);
	// assign the address of each assets so tempSpritePairs won't be affected by the container in the example application
	for (unsigned int i = 0; i < pairs; ++i)
	{
		TempSpritePairContainer[i].first = i_spriteDataGroups[i].Effect;
		TempSpritePairContainer[i].second = i_spriteDataGroups[i].Sprite;

		TempSpritePairContainer[i].first->IncrementReferenceCount();
		TempSpritePairContainer[i].second->IncrementReferenceCount();

		TempSpriteTextureContainer[i] = i_spriteDataGroups[i].Texture;
		TempSpriteTextureContainer[i]->IncrementReferenceCount();
	}

	pairs = static_cast<unsigned int>(i_opaqueMeshDataGroups.size());
	TempOpaqueMeshPairContainer.resize(pairs);
	TempOpaqueMeshTextureContainer.resize(pairs);
	// assign the address of each assets so tempMeshPairs won't be affected by the container in the example application
	for (unsigned int i = 0; i < pairs; ++i)
	{
		TempOpaqueMeshPairContainer[i].first = i_opaqueMeshDataGroups[i].Effect;
		TempOpaqueMeshPairContainer[i].second = i_opaqueMeshDataGroups[i].Mesh;

		TempOpaqueMeshPairContainer[i].first->IncrementReferenceCount();
		TempOpaqueMeshPairContainer[i].second->IncrementReferenceCount();

		TempOpaqueMeshTextureContainer[i] = i_opaqueMeshDataGroups[i].Texture;
		TempOpaqueMeshTextureContainer[i]->IncrementReferenceCount();
	}

	pairs = static_cast<unsigned int>(i_transMeshDataGroups.size());
	TempTransMeshPairContainer.resize(pairs);
	TempTransMeshTextureContainer.resize(pairs);
	// assign the address of each assets so tempMeshPairs won't be affected by the container in the example application
	for (unsigned int i = 0; i < pairs; ++i)
	{
		TempTransMeshPairContainer[i].first = i_transMeshDataGroups[i].Effect;
		TempTransMeshPairContainer[i].second = i_transMeshDataGroups[i].Mesh;

		TempTransMeshPairContainer[i].first->IncrementReferenceCount();
		TempTransMeshPairContainer[i].second->IncrementReferenceCount();

		TempTransMeshTextureContainer[i] = i_transMeshDataGroups[i].Texture;
		TempTransMeshTextureContainer[i]->IncrementReferenceCount();
	}

	ClearState = eae6320::Graphics::EClearState::NotClear;
}

void sDataRequiredToRenderAFrame::ClearContainer()
{
	if (ClearState == eae6320::Graphics::EClearState::NotClear)
	{
		// decrease ref after use
		unsigned int pairs = static_cast<unsigned int>(TempSpritePairContainer.size());
		for (unsigned int i = 0; i < pairs; ++i)
		{
			TempSpritePairContainer[i].first->DecrementReferenceCount();
			TempSpritePairContainer[i].second->DecrementReferenceCount();

			TempSpriteTextureContainer[i]->DecrementReferenceCount();
		}

		pairs = static_cast<unsigned int>(TempOpaqueMeshTextureContainer.size());
		for (unsigned int i = 0; i < pairs; ++i)
		{
			TempOpaqueMeshPairContainer[i].first->DecrementReferenceCount();
			TempOpaqueMeshPairContainer[i].second->DecrementReferenceCount();

			TempOpaqueMeshTextureContainer[i]->DecrementReferenceCount();
		}

		pairs = static_cast<unsigned int>(TempTransMeshPairContainer.size());
		for (unsigned int i = 0; i < pairs; ++i)
		{
			TempTransMeshPairContainer[i].first->DecrementReferenceCount();
			TempTransMeshPairContainer[i].second->DecrementReferenceCount();

			TempTransMeshTextureContainer[i]->DecrementReferenceCount();
		}
		ClearState = eae6320::Graphics::EClearState::Clear;
	}
}
