#ifndef EAE6320_CENEMY_H
#define EAE6320_CENEMY_H

// Include Files
//==============

#include <Engine/Results/Results.h>
#include <Engine/Math/cMatrix_transformation.h>

#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Graphics/Graphics.h>

namespace eae6320
{
	namespace Graphics
	{
		class cEffect;
		class cMesh;
		class cTexture;
	}
}

namespace eae6320
{
	class cEnemy
	{
	public:
		static const char* vertShaderPath;
		static const char* fragShaderPath;

		static Graphics::cMesh* enemyMesh;
		static Graphics::cTexture* enemyTexture;

		static void RegisteGraphicsInfo(Graphics::cMesh *i_enemyMesh, Graphics::cTexture *i_enemyTexture);
		static cEnemy* SpawnEnemy(Math::sVector i_position, Math::sVector i_velocity);

	public:
		void Update(float i_lastFrameTime);

		void Destroy();
		Graphics::MeshDataGroup GetMeshDataGroup() { return meshDataGroup_; }
		Math::sVector GetPosition() { return rigidBody_.position; }

	private:
		cEnemy(Graphics::cEffect *i_effectInstance, Math::sVector i_position, Math::sVector i_velocity);

		Physics::sRigidBodyState rigidBody_;
		Graphics::MeshDataGroup meshDataGroup_;
	};
}

#endif	// EAE6320_CENEMY_H
