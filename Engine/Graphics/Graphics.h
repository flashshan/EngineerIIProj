/*
	This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Include Files
//==============

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>

#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>
#include <Engine/Math/cMatrix_transformation.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

#include <vector>
#include "cTexture.h"

namespace eae6320
{
	namespace Graphics
	{
		class cEffect;
		class cSprite;
		class cMesh;

		struct SpriteDataGroup {
			cEffect *Effect;
			cSprite *Sprite;
			cTexture *Texture;
			SpriteDataGroup() 
				: Effect(nullptr), Sprite(nullptr), Texture(nullptr)
			{
			}

			SpriteDataGroup(cEffect *i_effect, cSprite *i_sprite, cTexture *i_texture)
				: Effect(i_effect), Sprite(i_sprite), Texture(i_texture)
			{
			}
		};

		struct MeshDataGroup {
		public:
			cEffect *Effect;
			cMesh *Mesh;
			cTexture *Texture;
			Math::sVector Position;
			Math::cQuaternion Rotation;
			float ZDepth;

			MeshDataGroup()
				: Effect(nullptr), Mesh(nullptr), Texture(nullptr)
			{
			}

			MeshDataGroup(cEffect *i_effect, cMesh *i_mesh, cTexture *i_texture)
				: Effect(i_effect), Mesh(i_mesh), Texture(i_texture)
			{
			}	  
			static bool CompareRenderOrder(const MeshDataGroup &i_left, const MeshDataGroup &i_right)
			{
				return i_left.ZDepth < i_right.ZDepth;
			}
		};

	}
}

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		// Submission
		//-----------

		// These functions should be called from the application (on the application loop thread)

		// As the class progresses you will add your own functions for submitting data,
		// but the following is an example (that gets called automatically)
		// of how the application submits the total elapsed times
		// for the frame currently being submitted
		void SubmitElapsedTime( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime );
		
		void SubmitCameraData(const Math::cMatrix_transformation &i_cameraProjectedMatrix, const Math::sVector &i_cameraPos, const Math::cQuaternion &i_cameraQuat);

		void SubmitRenderData(const float* i_backGroundColor, const std::vector<SpriteDataGroup> &i_spriteDataGroups, const std::vector<MeshDataGroup> &i_opaqueMeshDataGroups, const std::vector<MeshDataGroup> &i_transMeshDataGroups);
		// When the application is ready to submit data for a new frame
		// it should call this before submitting anything
		// (or, said another way, it is not safe to submit data for a new frame
		// until this function returns successfully)
		cResult WaitUntilDataForANewFrameCanBeSubmitted( const unsigned int i_timeToWait_inMilliseconds );
		// When the application has finished submitting data for a frame
		// it must call this function
		cResult SignalThatAllDataForAFrameHasBeenSubmitted();

		// Render
		//-------

		// This is called (automatically) from the main/render thread.
		// It will render a submitted frame as soon as it is ready
		// (i.e. as soon as SignalThatAllDataForAFrameHasBeenSubmitted() has been called)
		void RenderFrame();

		// Initialization / Clean Up
		//--------------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow = NULL;
	#if defined( EAE6320_PLATFORM_D3D )
			uint16_t resolutionWidth, resolutionHeight;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication = NULL;
	#endif
#endif
		};

		cResult Initialize( const sInitializationParameters& i_initializationParameters );
		cResult CleanUp();

		// clear the data and ref inside buffers, should be done before application cleanUp
		void ClearBuffers();
	}
}

#endif	// EAE6320_GRAPHICS_H
