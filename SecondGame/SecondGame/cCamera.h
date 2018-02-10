#ifndef EAE6320_CCAMERA_H
#define EAE6320_CCAMERA_H

// Include Files
//==============

#include <Engine/Results/Results.h>
#include <Engine/Math/cMatrix_transformation.h>

#include <Engine/Physics/sRigidBodyState.h>


namespace eae6320
{
	class cCamera
	{
	public:
		cResult Initialize(const Math::sVector &i_position, float i_angularSpeed, float i_verticalFieldOfView_inDegree, float i_aspectRatio, float i_z_nearPlane, float i_z_farPlane);
		
		void UpdateSimulationBasedOnInput(float i_lastFrameTime);
																														
		void SubmitData(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate);

		cResult CleanUp();

	private:

		// for camera
		eae6320::Physics::sRigidBodyState cameraRigidBody_;
		eae6320::Math::cMatrix_transformation cameraProjectedTransform_;
	};
}

#endif	// EAE6320_CCAMERA_H
