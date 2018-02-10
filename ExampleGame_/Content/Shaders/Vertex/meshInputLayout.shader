/*
	This vertex shader is used to create a Direct3D vertex input layout object
*/

#include <Shaders/shaders.inc>

// Entry Point
//============
#if defined( EAE6320_PLATFORM_D3D )

void main(

	// Input
	//======

	// The "semantics" (the keywords in all caps after the colon) are arbitrary,
	// but must match the C call to CreateInputLayout()

	in const float3 i_position : POSITION,
	in const float4 i_color : COLOR0,
	in const float2 i_uv : TEXCOORD0,

	// Output
	//=======

	// An SV_POSITION value must always be output from every vertex shader
	// so that the GPU can figure out which fragments need to be shaded
	out float4 o_position : SV_POSITION,
	out float4 o_color : COLOR0,
	out float2 o_uv : TEXCOORD0

	)
{
	// The shader program is only used to generate a vertex input layout object;
	// the actual shading code is never used
	o_position = float4( i_position, 1.0 );
	o_color = i_color;
	o_uv = i_uv;
}

#endif