#include <Shaders/shaders.inc>


DeclareConstantBuffer(g_constantBuffer_perDrawCall, 2)
{
	float4x4 g_transform_localToWorld;
};


#if defined( EAE6320_PLATFORM_D3D )

void main(

	// Input
	//=====
	in const float3 i_position : POSITION,
	in const float4 i_color : COLOR0,
	in const float2 i_uv : TEXCOORD0,

	// Output
	//=======
	out float4 o_position : SV_POSITION,
	out float4 o_color : COLOR0,
	out float2 o_uv : TEXCOORD0
)

#elif defined( EAE6320_PLATFORM_GL )

// Input
//======
layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec2 i_uv;

// Output
//=======
layout(location = 0) out vec4 o_color;
layout(location = 1) out vec2 o_uv;

void main()

#endif
// Calculate the position of this vertex on screen
{
	/*float4 vertexPosition_local = float4(i_position, 1.0);
	float4 vertexPosition_world = Mul(g_transform_localToWorld, vertexPosition_local);
	float4 vertexPosition_camera = Mul(g_transform_worldToCamera, vertexPosition_world);
	o_position = Mul(g_transform_cameraToProjected, vertexPosition_camera);*/

	// for optional challenge
	o_position = Mul(g_transform_localToWorld, float4(i_position, 1.0));

	o_color = i_color;
	o_uv = i_uv;
}

// EAE6320_TODO: Change the position based on time!
// The value g_elapsedSecondCount_simulationTime constantly changes as the simulation progresses, and so by doing something like:
//	sin( g_elapsedSecondCount_simulationTime ) or cos( g_elapsedSecondCount_simulationTime )
// you can get a value that will oscillate between [-1,1].
// You should change at least one position element so that the triangle animates.
// For example, to change X you would do something kind of like:
//		o_position.x = ? sin( g_elapsedSecondCount_simulationTime ) ?
// You can change .x and .y (but leave .z as 0.0 and .w as 1.0).
// The screen dimensions are already [1,1], so you may want to do some math
// on the result of the sinusoid function to keep the triangle mostly on screen.
