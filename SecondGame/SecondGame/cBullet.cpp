#include "cBullet.h"

#include <Engine/Math/Functions.h>
#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>

#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/cTexture.h>


namespace eae6320
{
	const char* cBullet::vertShaderPath = "data/Shaders/Vertex/meshVert.shd";
	const char* cBullet::fragShaderPath = "data/Shaders/Fragment/meshFrag.shd";
	
	Graphics::cMesh* cBullet::bulletMesh = nullptr;
	Graphics::cTexture* cBullet::bulletTexture = nullptr;

	void cBullet::RegisteGraphicsInfo(Graphics::cMesh *i_bulletMesh, Graphics::cTexture *i_bulletTexture)
	{
		cBullet::bulletMesh = i_bulletMesh;
		cBullet::bulletTexture = i_bulletTexture;
	}


	cBullet* cBullet::SpawnBullet(Math::sVector i_position, Math::sVector i_velocity)
	{
		Graphics::cEffect *effectInstance;
		if (!(Graphics::cEffect::MakeEffect(effectInstance, cBullet::vertShaderPath, cBullet::fragShaderPath, Graphics::RenderStates::eRenderState::DepthBuffering)))
		{
			return nullptr;
		}

		return new cBullet(effectInstance, i_position, i_velocity);
	}


	cBullet::cBullet(Graphics::cEffect *i_effectInstance, Math::sVector i_position, Math::sVector i_velocity)
		: meshDataGroup_(Graphics::MeshDataGroup(i_effectInstance, cBullet::bulletMesh, cBullet::bulletTexture))
	{
		rigidBody_.position = i_position;
		rigidBody_.velocity = i_velocity;

		meshDataGroup_.Position = rigidBody_.position;
		meshDataGroup_.Rotation = rigidBody_.orientation;
	}

	void cBullet::Update(float i_lastFrameTime)
	{
		rigidBody_.Update(i_lastFrameTime);
		meshDataGroup_.Position = rigidBody_.position;
		meshDataGroup_.Rotation = rigidBody_.orientation;
	}

	void cBullet::Destroy()
	{
		delete this;
	}
}



