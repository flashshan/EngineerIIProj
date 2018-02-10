#include "cEnemy.h"

#include <Engine/Math/Functions.h>
#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>

#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/cTexture.h>


namespace eae6320
{
	const char* cEnemy::vertShaderPath = "data/Shaders/Vertex/meshVert.shd";
	const char* cEnemy::fragShaderPath = "data/Shaders/Fragment/meshFrag.shd";
	
	Graphics::cMesh* cEnemy::enemyMesh = nullptr;
	Graphics::cTexture* cEnemy::enemyTexture = nullptr;

	void cEnemy::RegisteGraphicsInfo(Graphics::cMesh *i_enemyMesh, Graphics::cTexture *i_enemyTexture)
	{
		cEnemy::enemyMesh = i_enemyMesh;
		cEnemy::enemyTexture = i_enemyTexture;
	}

	cEnemy* cEnemy::SpawnEnemy(Math::sVector i_position, Math::sVector i_velocity)
	{
		Graphics::cEffect *effectInstance;
		if (!(Graphics::cEffect::MakeEffect(effectInstance, cEnemy::vertShaderPath, cEnemy::fragShaderPath, Graphics::RenderStates::eRenderState::DepthBuffering)))
		{
			return nullptr;
		}

		return new cEnemy(effectInstance, i_position, i_velocity);
	}


	cEnemy::cEnemy(Graphics::cEffect *i_effectInstance, Math::sVector i_position, Math::sVector i_velocity)
		: meshDataGroup_(Graphics::MeshDataGroup(i_effectInstance, cEnemy::enemyMesh, cEnemy::enemyTexture))
	{
		rigidBody_.position = i_position;
		rigidBody_.velocity = i_velocity;

		meshDataGroup_.Position = rigidBody_.position;
		meshDataGroup_.Rotation = rigidBody_.orientation;
	}

	void cEnemy::Update(float i_lastFrameTime)
	{
		rigidBody_.Update(i_lastFrameTime);
		meshDataGroup_.Position = rigidBody_.position;
		meshDataGroup_.Rotation = rigidBody_.orientation;
	}

	void cEnemy::Destroy()
	{
		delete this;
	}
}



