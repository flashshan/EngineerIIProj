/*
	This vertex shader is used to create a Direct3D vertex input layout object
*/

#include <Shaders/shaders.inc>

#if defined( EAE6320_PLATFORM_D3D )

// Entry Point
//============

void main(

	// Input
	//======

	// The "semantics" (the keywords in all caps after the colon) are arbitrary,
	// but must match the C call to CreateInputLayout()

	in const float2 i_position : POSITION,
	in const float2 i_uv : TEXCOORD0,

	// Output
	//=======

	// An SV_POSITION value must always be output from every vertex shader
	// so that the GPU can figure out which fragments need to be shaded
	out float4 o_position : SV_POSITION,
	out float2 o_uv : TEXCOORD0

	)
{
	// The shader program is only used to generate a vertex input layout object;
	// the actual shading code is never used
	o_position = float4( i_position, 0.0, 1.0 );
	o_uv = i_uv;
}

#endif