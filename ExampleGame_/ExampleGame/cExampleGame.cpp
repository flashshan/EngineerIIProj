// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>

#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/cSprite.h>

#include <Engine/Math/Functions.h>
#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>

#include <algorithm>    // for random_shuffle
#include <time.h>

// Inherited Implementation
//=========================

void eae6320::cExampleGame::InitAllData()
{
	// init shaders
	int vertShaderNumber = 2;
	vertShaderPaths_.resize(vertShaderNumber);
	vertShaderPaths_[0] = new char[50]{ "data/Shaders/Vertex/spriteVert.shd" };
	vertShaderPaths_[1] = new char[50]{ "data/Shaders/Vertex/meshVert.shd" };

	int fragShaderNumber = 5;
	fragShaderPaths_.resize(fragShaderNumber);
	fragShaderPaths_[0] = new char[50]{ "data/Shaders/Fragment/spriteFrag.shd" };
	fragShaderPaths_[1] = new char[50]{ "data/Shaders/Fragment/spriteColorFrag.shd" };
	fragShaderPaths_[2] = new char[50]{ "data/Shaders/Fragment/spriteTransFrag.shd" };
	fragShaderPaths_[3] = new char[50]{ "data/Shaders/Fragment/meshFrag.shd" };
	fragShaderPaths_[4] = new char[50]{ "data/Shaders/Fragment/meshTransFrag.shd" };


	// init data for sprite
	spriteVertexCount_.resize(totalSprites_);
	for (unsigned int i = 0; i < spriteVertexCount_.size(); ++i)
		spriteVertexCount_[i] = 4;

	spriteVertexData_.resize(totalSprites_);
	for (unsigned int i = 0; i < spriteVertexData_.size(); ++i)
		spriteVertexData_[i] = new eae6320::Graphics::VertexFormats::sSprite[spriteVertexCount_[i]];

	CalcRectangleVertexDataFromUserInput(EAlignmentXType::ECenter, EAlignmentYType::EBottom, -0.2f, 0.05f, 0.4f, 0.1f, spriteVertexData_[0]);
	CalcRectangleVertexDataFromUserInput(EAlignmentXType::ECenter, EAlignmentYType::EBottom, -0.2f, 0.05f, 0.4f, 0.1f, spriteVertexData_[1]);
	CalcRectangleVertexDataFromUserInput(EAlignmentXType::ECenter, EAlignmentYType::EBottom, -0.1f, 0.18f, 0.2f, 0.1f, spriteVertexData_[2]);

}

void eae6320::cExampleGame::ClearAllData()
{
	// clear vertex data and shaders
	for (unsigned int i = 0; i < totalSprites_; ++i)
		delete[] spriteVertexData_[i];

	for (unsigned int i = 0; i < vertShaderPaths_.size(); ++i)
		delete vertShaderPaths_[i];
	for (unsigned int i = 0; i < fragShaderPaths_.size(); ++i)
		delete fragShaderPaths_[i];

	vertShaderPaths_.clear();
	fragShaderPaths_.clear();

	spriteVertexCount_.clear();
	spriteVertexData_.clear();
}

eae6320::cResult eae6320::cExampleGame::InitEffectAndSpritePairs()
{
	eae6320::cResult result = Results::Success;

	Graphics::cEffect *effectInstance = nullptr;
	Graphics::cSprite *spriteInstance = nullptr;

	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[0], fragShaderPaths_[0])))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = Graphics::cSprite::MakeSprite(spriteInstance, spriteVertexData_[0], spriteVertexCount_[0])))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	spriteDataGroups_.push_back(Graphics::SpriteDataGroup(effectInstance, spriteInstance, Graphics::cTexture::s_manager.Get(textureHandleContainer_[10].second)));

	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[0], fragShaderPaths_[0])))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = Graphics::cSprite::MakeSprite(spriteInstance, spriteVertexData_[1], spriteVertexCount_[1])))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	spriteDataGroups_.push_back(Graphics::SpriteDataGroup(effectInstance, spriteInstance, Graphics::cTexture::s_manager.Get(textureHandleContainer_[11].second)));

	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[0], fragShaderPaths_[0])))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = Graphics::cSprite::MakeSprite(spriteInstance, spriteVertexData_[2], spriteVertexCount_[2])))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	spriteDataGroups_.push_back(Graphics::SpriteDataGroup(effectInstance, spriteInstance, Graphics::cTexture::s_manager.Get(textureHandleContainer_[12].second)));


OnExit:
	return result;
}



eae6320::cResult eae6320::cExampleGame::ClearEffectAndSpritePairs()
{
	eae6320::cResult result = Results::Success;

	for (auto it = spriteDataGroups_.begin(); it != spriteDataGroups_.end(); ++it)
	{
		(*it).Effect->DecrementReferenceCount();
		(*it).Sprite->DecrementReferenceCount();
	}

	spriteDataGroups_.clear();
	return result;
}


eae6320::cResult eae6320::cExampleGame::InitEffectAndMeshPairs()
{
	eae6320::cResult result = Results::Success;

	Graphics::cEffect *effectInstance = nullptr;

	//mesh 1
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[0].second)));

	// mesh 2
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[1].second)));

	// mesh 3
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[2].second)));

	// mesh 4
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[3].second)));

	// mesh 5
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[4].second)));

	// mesh 6
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[5].second)));

	// mesh 7
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[6].second)));

	// mesh 8
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[7].second)));

	// mesh 9
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[8].second)));

OnExit:
	return result;
}


eae6320::cResult eae6320::cExampleGame::ClearEffectAndMeshPairs()
{
	eae6320::cResult result = Results::Success;

	for (auto it = opaqueMeshDataGroups_.begin(); it != opaqueMeshDataGroups_.end(); ++it)
	{
		(*it).Effect->DecrementReferenceCount();
		(*it).Mesh->DecrementReferenceCount();
	}
	opaqueMeshDataGroups_.clear();

	for (auto it = transMeshDataGroups_.begin(); it != transMeshDataGroups_.end(); ++it)
	{
		(*it).Effect->DecrementReferenceCount();
		(*it).Mesh->DecrementReferenceCount();
	}
	transMeshDataGroups_.clear();

	return result;
}




eae6320::cResult eae6320::cExampleGame::InitTextures()
{
	eae6320::cResult result = Results::Success;

	textureHandleContainer_.resize(13);

	// use bool to record whether the texture is successfully loaded, used inside clearTextures
	for (unsigned int i = 0; i < textureHandleContainer_.size(); ++i)
	{
		textureHandleContainer_[i].first = false;
	}

	// init textures
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture11.tex", textureHandleContainer_[0].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[0].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture12.tex", textureHandleContainer_[1].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[1].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture13.tex", textureHandleContainer_[2].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[2].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture21.tex", textureHandleContainer_[3].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[3].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture22.tex", textureHandleContainer_[4].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[4].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture23.tex", textureHandleContainer_[5].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[5].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture31.tex", textureHandleContainer_[6].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[6].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture32.tex", textureHandleContainer_[7].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[7].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/picture33.tex", textureHandleContainer_[8].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[8].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/white.tex", textureHandleContainer_[9].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[9].first = true;

	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/Start.tex", textureHandleContainer_[10].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[10].first = true;

	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/Restart.tex", textureHandleContainer_[11].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[11].first = true;

	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/YouWin.tex", textureHandleContainer_[12].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[12].first = true;

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::ClearTextures()
{
	eae6320::cResult result = Results::Success;

	// clear textures
	for (unsigned int i = 0; i < textureHandleContainer_.size(); ++i)
	{
		if (textureHandleContainer_[i].first)		// if successfully init
		{
			if (!(result = Graphics::cTexture::s_manager.Release(textureHandleContainer_[i].second)))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
		}
	}

OnExit:
	textureHandleContainer_.clear();
	return result;
}

eae6320::cResult eae6320::cExampleGame::InitMeshes()
{
	eae6320::cResult result = Results::Success;

	meshHandleContainer_.resize(3);

	// use bool to record whether the texture is successfully loaded, used inside clearTextures
	for (unsigned int i = 0; i < meshHandleContainer_.size(); ++i)
	{
		meshHandleContainer_[i].first = false;
	}

	// init meshes
	if (!(result = Graphics::cMesh::s_manager.Load("data/Meshes/planeVertMaya.mesh", meshHandleContainer_[0].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	meshHandleContainer_[0].first = true;

	/*if (!(result = Graphics::cMesh::s_manager.Load("data/Meshes/planeMaya.mesh", meshHandleContainer_[1].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	meshHandleContainer_[1].first = true;
	if (!(result = Graphics::cMesh::s_manager.Load("data/Meshes/circleMaya.mesh", meshHandleContainer_[2].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	meshHandleContainer_[2].first = true;*/

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::ClearMeshes()
{
	eae6320::cResult result = Results::Success;

	// clear meshes
	for (unsigned int i = 0; i < meshHandleContainer_.size(); ++i)
	{
		if (meshHandleContainer_[i].first)		// if successfully init
		{
			if (!(result = Graphics::cMesh::s_manager.Release(meshHandleContainer_[i].second)))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
		}
	}

OnExit:
	meshHandleContainer_.clear();
	return result;
}


void eae6320::cExampleGame::SetBackgroundColor(float i_r, float i_g, float i_b, float i_a)
{
	backgroundColor_[0] = i_r;
	backgroundColor_[1] = i_g;
	backgroundColor_[2] = i_b;
	backgroundColor_[3] = i_a;
}


void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	// submit camera data
	mainCamera_.SubmitData(i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	tempBackgroundColor_[0] = backgroundColor_[0] + 0.2f * sinf(totalSimuTime_);
	tempBackgroundColor_[1] = backgroundColor_[1] + 0.2f * cosf(totalSimuTime_);
	tempBackgroundColor_[2] = backgroundColor_[2] + 0.2f * sinf(totalSimuTime_);
	tempBackgroundColor_[3] = backgroundColor_[3];

	Graphics::SubmitRenderData(&tempBackgroundColor_[0], tempSpriteDataGroups_, opaqueMeshDataGroups_, transMeshDataGroups_);
}


// Run
//----

void eae6320::cExampleGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Escape))
	{
		// Exit the application
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	}

	// slow down simulation based on Input(Space)
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
		SetSimulationRate(0.5f);
	else
		SetSimulationRate(1.0f);

	if (currentGameState_ == GameState::GameStop && UserInput::IsKeyPressed('R'))
	{
		if (!rPressState_)
		{
			GameStart();
			rPressState_ = true;
		}
	}
	else
	{
		rPressState_ = false;
	}

	if (currentGameState_ == GameState::GameRun && UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		if (!upPressState_)
		{
			if (currentWhitePosition_ < 6)
			{
				std::swap(puzzleState_[currentWhitePosition_], puzzleState_[currentWhitePosition_ + 3]);
				currentWhitePosition_ += 3;
				UpdateMeshPosition();
				if (CheckPuzzleState())
					GameWin();
			}
			upPressState_ = true;
		}
	}
	else
	{
		upPressState_ = false;
	}
	if (currentGameState_ == GameState::GameRun && UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		if (!downPressState_)
		{
			if (currentWhitePosition_ > 2)
			{
				std::swap(puzzleState_[currentWhitePosition_], puzzleState_[currentWhitePosition_ - 3]);
				currentWhitePosition_ -= 3;
				UpdateMeshPosition();
				if (CheckPuzzleState())
					GameWin();
			}
			downPressState_ = true;
		}
	}
	else
	{
		downPressState_ = false;
	}
	if (currentGameState_ == GameState::GameRun && UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		if (!leftPressState_)
		{
			if (currentWhitePosition_ % 3 != 2)
			{
				std::swap(puzzleState_[currentWhitePosition_], puzzleState_[currentWhitePosition_ + 1]);
				currentWhitePosition_ += 1;
				UpdateMeshPosition();
				if (CheckPuzzleState())
					GameWin();
			}
			leftPressState_ = true;
		}
	}
	else
	{
		leftPressState_ = false;
	}
	if (currentGameState_ == GameState::GameRun && UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		if (!rightPressState_)
		{
			if (currentWhitePosition_ % 3 != 0)
			{
					std::swap(puzzleState_[currentWhitePosition_], puzzleState_[currentWhitePosition_ - 1]);
				currentWhitePosition_ -= 1;
				UpdateMeshPosition();
				if (CheckPuzzleState())
					GameWin();
			}
			rightPressState_ = true;
		}
	}
	else
	{
		rightPressState_ = false;
	}
}

void eae6320::cExampleGame::UpdateSimulationBasedOnInput()
{
	//// actually update after predict render for last frame
	//simulatedRigidBody.Update(simulationTimeSinceLastFrame_);
	//
	//// set velocity and acceleration for simulated object
	//Math::sVector meshAcceleration = Math::sVector(0.0f, 0.0f, 0.0f);
	//float acceleration = 0.4f;
	//bool isMoveKeyHold = false;
	//if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	//{
	//	isMoveKeyHold = true;
	//	meshAcceleration.x = -acceleration;
	//}
	//else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	//{
	//	isMoveKeyHold = true;
	//	meshAcceleration.x = acceleration;
	//}

	//if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	//{
	//	isMoveKeyHold = true;
	//	meshAcceleration.y = -acceleration;
	//}
	//else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	//{
	//	isMoveKeyHold = true;
	//	meshAcceleration.y = acceleration;
	//}
	//simulatedRigidBody.acceleration = meshAcceleration;

	//if (!isMoveKeyHold)
	//{
	//	simulatedRigidBody.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
	//}

	mainCamera_.UpdateSimulationBasedOnInput(simulationTimeSinceLastFrame_);
}



void eae6320::cExampleGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	totalSimuTime_ += i_elapsedSecondCount_sinceLastUpdate;

	simulationTimeSinceLastFrame_ = i_elapsedSecondCount_sinceLastUpdate;
}


// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	eae6320::cResult result = Results::Success;

	totalSprites_ = 3;
	//currentOpaqueMeshes_ = 9;
	//currentTransMeshes_ = 0;

	totalSimuTime_ = 0.0f;

	// init all rendered data
	InitAllData();

	// init main camera
	uint16_t currentWidth, currentHeight;
	GetCurrentResolution(currentWidth, currentHeight);
	if (!(result = mainCamera_.Initialize(Math::sVector(0.0f, -2.0f, 16.0f), 0.0f, 45.0f, (float)currentWidth / (float)currentHeight, 0.1f, 100.0f)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	//init textures	and mesh
	if (!(result = InitTextures()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = InitMeshes()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	// init effects and sprites
	if (!(result = InitEffectAndSpritePairs()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	//init textures
	if (!(result = InitEffectAndMeshPairs()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	SetBackgroundColor(0.8f, 0.8f, 0.8f, 1.0f);

	tempSpriteDataGroups_.clear();
	tempSpriteDataGroups_.push_back(spriteDataGroups_[0]);

	// init game
	piecePosition_[0] = Math::sVector(-0.2f, 0.2f, 0.0f);
	piecePosition_[1] = Math::sVector(0.0f, 0.2f, 0.0f);
	piecePosition_[2] = Math::sVector(0.2f, 0.2f, 0.0f);
	piecePosition_[3] = Math::sVector(-0.2f, 0.0f, 0.0f);
	piecePosition_[4] = Math::sVector(0.0f, 0.0f, 0.0f);
	piecePosition_[5] = Math::sVector(0.2f, 0.0f, 0.0f);
	piecePosition_[6] = Math::sVector(-0.2f, -0.2f, 0.0f);
	piecePosition_[7] = Math::sVector(0.0f, -0.2f, 0.0f);
	piecePosition_[8] = Math::sVector(0.2f, -0.2f, 0.0f);

	puzzleState_.reserve(9);
	for (int i = 0; i < 9; ++i)
	{
		puzzleState_.push_back(i);
	}
	UpdateMeshPosition();

	currentGameState_ = GameState::GameStop;

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	eae6320::cResult result = Results::Success;

	// clean graphics buffer ref count
	eae6320::Graphics::ClearBuffers();

	// clean up effects and mesh
	if (!(result = ClearEffectAndMeshPairs()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	// clean up effects and sprite
	if (!(result = ClearEffectAndSpritePairs()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	// clean up main camera
	if (!(result = mainCamera_.CleanUp()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	// clean up textures and meshes
	if (!(result = ClearTextures()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = ClearMeshes()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	ClearAllData();

OnExit:
	return result;
}


void eae6320::cExampleGame::GameStart()
{
	srand(unsigned int(time(0)));
	std::random_shuffle(puzzleState_.begin(), puzzleState_.end() - 1); // don't random the last elements

	int reverseNumber = 0;
	for (int i = 0; i < 7; ++i)
	{
		for (int j = i + 1; j < 8; ++j)
		{
			if (puzzleState_[i] > puzzleState_[j])
				++reverseNumber;
		}
	}
	
	if (reverseNumber % 2 == 1)
	{
		std::swap(puzzleState_[7], puzzleState_[rand() % 7]);
	}

	if (CheckPuzzleState())	 // if after randomness, you win (very very tiny possiblities)
	{
		GameStart();
	}
	else
	{
		currentWhitePosition_ = 8;
		opaqueMeshDataGroups_[8].Texture = Graphics::cTexture::s_manager.Get(textureHandleContainer_[9].second);
		UpdateMeshPosition();
		tempSpriteDataGroups_.clear();
		currentGameState_ = GameState::GameRun;
	}
}

void eae6320::cExampleGame::GameWin()
{
	opaqueMeshDataGroups_[8].Texture = Graphics::cTexture::s_manager.Get(textureHandleContainer_[8].second);

	tempSpriteDataGroups_.clear();
	tempSpriteDataGroups_.push_back(spriteDataGroups_[1]);
	tempSpriteDataGroups_.push_back(spriteDataGroups_[2]);

	currentGameState_ = GameState::GameStop;
}

bool eae6320::cExampleGame::CheckPuzzleState()
{
	for (int i = 0; i < 9; ++i)
	{
		if (puzzleState_[i] != i)
			return false;
	}
	return true;
}

void eae6320::cExampleGame::UpdateMeshPosition()
{
	for (int i = 0; i < 9; ++i)
	{
		opaqueMeshDataGroups_[puzzleState_[i]].Position = piecePosition_[i];
	}
}