#include "cCamera.h"

#include <Engine/Math/Functions.h>
#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>

#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>



eae6320::cResult eae6320::cCamera::Initialize(const Math::sVector &i_position, float i_angularSpeed, float i_verticalFieldOfView_inDegree, float i_aspectRatio, float i_z_nearPlane, float i_z_farPlane)
{
	// init camera
	cameraRigidBody_.velocity = i_position;
	cameraRigidBody_.angularSpeed = i_angularSpeed;
	cameraProjectedTransform_ = Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(Math::ConvertDegreesToRadians(i_verticalFieldOfView_inDegree), i_aspectRatio, i_z_nearPlane, i_z_farPlane);

	return eae6320::Results::Success;
}

void eae6320::cCamera::UpdateSimulationBasedOnInput(float i_lastFrameTime)
{
	// actually update after predict render for last frame
	cameraRigidBody_.Update(i_lastFrameTime);

	// set velocity and rotation for camera
	float cameraAngleSpeed = 1.0f;
	cameraRigidBody_.angularSpeed = 0.0f;
	if (UserInput::IsKeyPressed('Q'))
	{
		cameraRigidBody_.angularSpeed = cameraAngleSpeed;
	}
	else if (UserInput::IsKeyPressed('E'))
	{
		cameraRigidBody_.angularSpeed = -cameraAngleSpeed;
	}

	float cameraMoveSpeed = 1.0f;
	Math::cMatrix_transformation CameraInWorld = Math::cMatrix_transformation(cameraRigidBody_.orientation, cameraRigidBody_.position);
	cameraRigidBody_.velocity = Math::sVector(0.0f, 0.0f, 0.0f);

	if (UserInput::IsKeyPressed('A'))
	{
		cameraRigidBody_.velocity -= CameraInWorld.GetRightDirection() * cameraMoveSpeed;
	}
	else if (UserInput::IsKeyPressed('D'))
	{
		cameraRigidBody_.velocity += CameraInWorld.GetRightDirection() * cameraMoveSpeed;
	}

	if (UserInput::IsKeyPressed('S'))
	{
		cameraRigidBody_.velocity += CameraInWorld.GetBackDirection() * cameraMoveSpeed;
	}
	else if (UserInput::IsKeyPressed('W'))
	{
		cameraRigidBody_.velocity -= CameraInWorld.GetBackDirection() * cameraMoveSpeed;
	}
}

void eae6320::cCamera::SubmitData(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	Graphics::SubmitCameraData(cameraProjectedTransform_, cameraRigidBody_.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate), cameraRigidBody_.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate));
}

eae6320::cResult eae6320::cCamera::CleanUp()
{
	return eae6320::Results::Success;
}
