#ifndef EAE6320_CBULLET_H
#define EAE6320_CBULLET_H

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
	class cBullet
	{
	public:
		static const char* vertShaderPath;
		static const char* fragShaderPath;

		static Graphics::cMesh* bulletMesh;
		static Graphics::cTexture* bulletTexture;

		static void RegisteGraphicsInfo(Graphics::cMesh *i_bulletMesh, Graphics::cTexture *i_bulletTexture);
		static cBullet* SpawnBullet(Math::sVector i_position, Math::sVector i_velocity);
		
	public:
		void Update(float i_lastFrameTime);
	    
		void Destroy();

		Graphics::MeshDataGroup GetMeshDataGroup() { return meshDataGroup_; }
		Math::sVector GetPosition() { return rigidBody_.position; }

	private:
		cBullet(Graphics::cEffect *i_effectInstance, Math::sVector i_position, Math::sVector i_velocity);

		Physics::sRigidBodyState rigidBody_;
		Graphics::MeshDataGroup meshDataGroup_;	
	};
}

#endif	// EAE6320_CBULLET_H
