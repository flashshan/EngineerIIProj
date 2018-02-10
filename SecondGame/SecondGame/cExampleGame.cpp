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
	CalcRectangleVertexDataFromUserInput(EAlignmentXType::ECenter, EAlignmentYType::EBottom, -0.1f, 0.18f, 0.2f, 0.1f, spriteVertexData_[3]);


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
	spriteDataGroups_.push_back(Graphics::SpriteDataGroup(effectInstance, spriteInstance, Graphics::cTexture::s_manager.Get(textureHandleContainer_[5].second)));

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
	spriteDataGroups_.push_back(Graphics::SpriteDataGroup(effectInstance, spriteInstance, Graphics::cTexture::s_manager.Get(textureHandleContainer_[6].second)));

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
	spriteDataGroups_.push_back(Graphics::SpriteDataGroup(effectInstance, spriteInstance, Graphics::cTexture::s_manager.Get(textureHandleContainer_[7].second)));

	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[0], fragShaderPaths_[0])))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = Graphics::cSprite::MakeSprite(spriteInstance, spriteVertexData_[3], spriteVertexCount_[3])))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	spriteDataGroups_.push_back(Graphics::SpriteDataGroup(effectInstance, spriteInstance, Graphics::cTexture::s_manager.Get(textureHandleContainer_[8].second)));


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

	//mesh player
	if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[0].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[2].second)));

	// mesh background
	/*if (!(result = Graphics::cEffect::MakeEffect(effectInstance, vertShaderPaths_[1], fragShaderPaths_[3], Graphics::RenderStates::eRenderState::DepthBuffering)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	opaqueMeshDataGroups_.push_back(Graphics::MeshDataGroup(effectInstance, Graphics::cMesh::s_manager.Get(meshHandleContainer_[2].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[3].second)));
	opaqueMeshDataGroups_[1].Position = Math::sVector(0.0f, 0.0f, -1.0f);*/


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

	textureHandleContainer_.resize(9);

	// use bool to record whether the texture is successfully loaded, used inside clearTextures
	for (unsigned int i = 0; i < textureHandleContainer_.size(); ++i)
	{
		textureHandleContainer_[i].first = false;
	}

	// init textures
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/white.tex", textureHandleContainer_[0].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[0].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/red.tex", textureHandleContainer_[1].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[1].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/green.tex", textureHandleContainer_[2].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[2].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/blue.tex", textureHandleContainer_[3].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[3].first = true;
	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/yellow.tex", textureHandleContainer_[4].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[4].first = true;

	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/Start.tex", textureHandleContainer_[5].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[5].first = true;

	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/Restart.tex", textureHandleContainer_[6].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[6].first = true;

	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/YouWin.tex", textureHandleContainer_[7].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[7].first = true;

	if (!(result = Graphics::cTexture::s_manager.Load("data/Textures/YouLose.tex", textureHandleContainer_[8].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	textureHandleContainer_[8].first = true;

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

	meshHandleContainer_.resize(4);

	// use bool to record whether the texture is successfully loaded, used inside clearTextures
	for (unsigned int i = 0; i < meshHandleContainer_.size(); ++i)
	{
		meshHandleContainer_[i].first = false;
	}

	// init meshes
	if (!(result = Graphics::cMesh::s_manager.Load("data/Meshes/playerCube.mesh", meshHandleContainer_[0].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	meshHandleContainer_[0].first = true;

	if (!(result = Graphics::cMesh::s_manager.Load("data/Meshes/enemyCube.mesh", meshHandleContainer_[1].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	meshHandleContainer_[1].first = true;

	if (!(result = Graphics::cMesh::s_manager.Load("data/Meshes/backgroundPlane.mesh", meshHandleContainer_[2].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	meshHandleContainer_[2].first = true;

	if (!(result = Graphics::cMesh::s_manager.Load("data/Meshes/bulletPlane.mesh", meshHandleContainer_[3].second)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	meshHandleContainer_[3].first = true;

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

	/*tempBackgroundColor_[0] = backgroundColor_[0] + 0.2f * sinf(totalSimuTime_);
	tempBackgroundColor_[1] = backgroundColor_[1] + 0.2f * cosf(totalSimuTime_);
	tempBackgroundColor_[2] = backgroundColor_[2] + 0.2f * sinf(totalSimuTime_);
	tempBackgroundColor_[3] = backgroundColor_[3];*/

	opaqueMeshDataGroups_[0].Position = simulatedRigidBody.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate);
	opaqueMeshDataGroups_[0].Rotation = simulatedRigidBody.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate);

	opaqueMeshDataGroups_.resize(1 + enemies.size() + bullets.size());
	for (unsigned int i = 0; i < enemies.size(); ++i)
	{
		opaqueMeshDataGroups_[1 + i] = enemies[i]->GetMeshDataGroup();
	}
	for (unsigned int i = 0; i < bullets.size(); ++i)
	{
		opaqueMeshDataGroups_[1 + enemies.size() + i] = bullets[i]->GetMeshDataGroup();
	}

	Graphics::SubmitRenderData(&backgroundColor_[0], tempSpriteDataGroups_, opaqueMeshDataGroups_, transMeshDataGroups_);
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
	/*if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
		SetSimulationRate(0.5f);
	else
		SetSimulationRate(1.0f);*/

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

	if (currentGameState_ == GameState::GameRun && UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{
		if (!spacePressState_)
		{
			bullets.push_back(cBullet::SpawnBullet(simulatedRigidBody.position, Math::sVector(0.0f, 0.8f, 0.0f)));
			spacePressState_ = true;
		}
	}
	else
	{
		spacePressState_ = false;
	}
}

void eae6320::cExampleGame::UpdateSimulationBasedOnInput()
{
	// actually update after predict render for last frame
	simulatedRigidBody.Update(simulationTimeSinceLastFrame_);

	// set velocity and acceleration for simulated object
	Math::sVector meshVelocity = Math::sVector(0.0f, 0.0f, 0.0f);
	
	
	if (currentGameState_ == GameState::GameRun)
	{
		for (unsigned int i = 0; i < enemies.size(); ++i)
		{
			enemies[i]->Update(simulationTimeSinceLastFrame_);
		}
		for (unsigned int i = 0; i < bullets.size(); ++i)
		{
			bullets[i]->Update(simulationTimeSinceLastFrame_);
		}

		CheckBoundary();
		CheckCollisionState();

		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
		{
			meshVelocity.x = -playerSpeed_;
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
		{
			meshVelocity.x = playerSpeed_;
		}

		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
		{
			meshVelocity.y = -playerSpeed_;
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
		{
			meshVelocity.y = playerSpeed_;
		}
	}
	simulatedRigidBody.velocity = meshVelocity;

	mainCamera_.UpdateSimulationBasedOnInput(simulationTimeSinceLastFrame_);
}



void eae6320::cExampleGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	totalSimuTime_ += i_elapsedSecondCount_sinceLastUpdate;
	simulationTimeSinceLastFrame_ = i_elapsedSecondCount_sinceLastUpdate;

	if (currentGameState_ == GameState::GameRun)
	{
		timer_ += i_elapsedSecondCount_sinceLastUpdate;
		if (firstSpawnEnemy_)
		{
			if (timer_ > initInterval_)
			{
				timer_ -= initInterval_;
				enemies.push_back(cEnemy::SpawnEnemy(Math::sVector(Math::RandomRangeFloat(leftBoundary_, rightBoundary_), upBoundary_ - 0.1f, 0.0f), Math::sVector(0.0f, Math::RandomRangeFloat(-0.2f, -0.1f), 0.0f)));

				nextSpawnInterval_ = Math::RandomRangeFloat(spawnMinInterval_, spawnMaxInterval_);
				firstSpawnEnemy_ = false;
			}
		}
		else
		{
			if (timer_ > nextSpawnInterval_)
			{
				timer_ -= nextSpawnInterval_;
				enemies.push_back(cEnemy::SpawnEnemy(Math::sVector(Math::RandomRangeFloat(leftBoundary_, rightBoundary_), upBoundary_ - 0.1f, 0.0f), Math::sVector(0.0f, Math::RandomRangeFloat(-0.2f, -0.1f), 0.0f)));
				nextSpawnInterval_ = Math::RandomRangeFloat(spawnMinInterval_, spawnMaxInterval_);
			}
		}
	}

}


// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	eae6320::cResult result = Results::Success;

	totalSprites_ = 4;
	//currentOpaqueMeshes_ = 9;
	//currentTransMeshes_ = 0;

	totalSimuTime_ = 0.0f;

	// init all rendered data
	InitAllData();

	// init main camera
	uint16_t currentWidth, currentHeight;
	GetCurrentResolution(currentWidth, currentHeight);
	if (!(result = mainCamera_.Initialize(Math::sVector(0.0f, -2.0f, 20.0f), 0.0f, 45.0f, (float)currentWidth / (float)currentHeight, 0.1f, 100.0f)))
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

	SetBackgroundColor(0.6f, 0.85f, 0.92f, 1.0f);
	simulatedRigidBody.position = Math::sVector(0.0f, downBoundary_ + 0.3f, 0.0f);

	tempSpriteDataGroups_.clear();
	tempSpriteDataGroups_.push_back(spriteDataGroups_[0]);

	cEnemy::RegisteGraphicsInfo(Graphics::cMesh::s_manager.Get(meshHandleContainer_[1].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[1].second));
	cBullet::RegisteGraphicsInfo(Graphics::cMesh::s_manager.Get(meshHandleContainer_[3].second), Graphics::cTexture::s_manager.Get(textureHandleContainer_[4].second));

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

	firstSpawnEnemy_ = true;
	killNumber_ = 0;
	timer_ = 0.0f;

	tempSpriteDataGroups_.clear();
	currentGameState_ = GameState::GameRun;
}

void eae6320::cExampleGame::GameWin()
{
	for (unsigned int i = 0; i < enemies.size(); ++i)
		enemies[i]->Destroy();
	for (unsigned int i = 0; i < bullets.size(); ++i)
		bullets[i]->Destroy();

	enemies.clear();
	bullets.clear();

	tempSpriteDataGroups_.clear();
	tempSpriteDataGroups_.push_back(spriteDataGroups_[1]);
	tempSpriteDataGroups_.push_back(spriteDataGroups_[2]);

	currentGameState_ = GameState::GameStop;
}

void eae6320::cExampleGame::GameLose()
{
	for (unsigned int i = 0; i < enemies.size(); ++i)
		enemies[i]->Destroy();
	for (unsigned int i = 0; i < bullets.size(); ++i)
		bullets[i]->Destroy();

	enemies.clear();
	bullets.clear();

	tempSpriteDataGroups_.clear();
	tempSpriteDataGroups_.push_back(spriteDataGroups_[1]);
	tempSpriteDataGroups_.push_back(spriteDataGroups_[3]);

	currentGameState_ = GameState::GameStop;
}



void eae6320::cExampleGame::CheckBoundary()
{
	for (unsigned int i = 0; i < enemies.size(); ++i)
	{
		Math::sVector pos = enemies[i]->GetPosition();
		if (pos.y > upBoundary_ || pos.y < downBoundary_)
		{
			//enemies[i]->Destroy();
			std::swap(enemies[i], enemies[enemies.size() - 1]);
			enemies.pop_back();
			--i;
		}
	}

	for (unsigned int i = 0; i < bullets.size(); ++i)
	{
		Math::sVector pos = bullets[i]->GetPosition();
		if (pos.y > upBoundary_ || pos.y < downBoundary_)
		{
			//bullets[i]->Destroy();
			std::swap(bullets[i], bullets[bullets.size() - 1]);
			bullets.pop_back();
			--i;
		}
	}
}

void eae6320::cExampleGame::CheckCollisionState()
{
	Math::sVector playerPos = simulatedRigidBody.position;

	for (unsigned int i = 0; i < enemies.size(); ++i)
	{
		Math::sVector pos1 = enemies[i]->GetPosition();
		
		if (pos1.x - enemyExtend_ - playerExtend_ < playerPos.x && pos1.x + enemyExtend_ + playerExtend_ > playerPos.x &&
			pos1.y - enemyExtend_ - playerExtend_ < playerPos.y && pos1.y + enemyExtend_ + playerExtend_ > playerPos.y)
		{
			GameLose();
			return;
		}

		for (unsigned int j = 0; j < bullets.size(); ++j)
		{
			Math::sVector pos2 = bullets[j]->GetPosition();
			if (pos1.x - enemyExtend_ - bulletExtendX_ < pos2.x && pos1.x + enemyExtend_ + bulletExtendX_ > pos2.x &&
				pos1.y - enemyExtend_ - bulletExtendY_ < pos2.y && pos1.y + enemyExtend_ + bulletExtendY_ > pos2.y)
			{
				//enemies[i]->Destroy();
				std::swap(enemies[i], enemies[enemies.size() - 1]);
				enemies.pop_back();
				--i;

				//bullets[j]->Destroy();
				std::swap(bullets[j], bullets[bullets.size() - 1]);
				bullets.pop_back();
				++killNumber_;
				if (killNumber_ > goalKillNumber_)
				{
					GameWin();
					return;
				}
				break;
			}
		}
	}
}


