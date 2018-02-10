/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Types
//======

#if defined( EAE6320_PLATFORM_D3D )

	#define vec2 float2
	#define vec3 float3
	#define vec4 float4
	#define mat2 float2x2
	#define mat3 float3x3
	#define mat4 float4x4

#elif defined( EAE6320_PLATFORM_GL )

	#define float2 vec2
	#define float3 vec3
	#define float4 vec4
	#define float2x2 mat2
	#define float3x3 mat3
	#define float4x4 mat4

	#define o_position gl_Position

#endif



#if defined( EAE6320_PLATFORM_D3D )

	// Constant Buffers
	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )


	#define DeclareTexture(i_id) Texture2D g_color_texture : register(t##i_id); \
								 SamplerState g_color_samplerState : register(s##i_id);

	
	// Functions
	#define GetSampleColor(i_uv) g_color_texture.Sample(g_color_samplerState, i_uv);

	#define Mul(i_matrix, i_vector) mul( i_matrix, i_vector );



#elif defined( EAE6320_PLATFORM_GL )

	// Constant Buffers

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name

	#define DeclareTexture(i_id) layout(binding = i_id) uniform sampler2D g_color_texture;

	
	// Functions
	#define GetSampleColor(i_uv) texture2D(g_color_texture, i_uv);

	#define Mul(i_matrix, i_vector) i_matrix * i_vector;


#endif


DeclareConstantBuffer(g_constantBuffer_perFrame, 0)
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	float2 g_padding; // For float4 alignment
};