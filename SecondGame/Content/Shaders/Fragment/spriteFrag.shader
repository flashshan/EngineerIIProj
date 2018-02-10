#include <Shaders/shaders.inc>

DeclareTexture(0)

#if defined( EAE6320_PLATFORM_D3D )

void main(

	// Input
	in const float4 i_position : SV_POSITION,
	in const float2 i_uv : TEXCOORD0,

	// Output
	out float4 o_color : SV_TARGET
)

#elif defined( EAE6320_PLATFORM_GL )

// Input
layout(location = 0) in vec2 i_uv;

// Output
out vec4 o_color;

void main()

#endif

{
	const float4 sampledColor = GetSampleColor(i_uv);

	o_color = sampledColor;

	// EAE6320_TODO: Change the color based on time!
	// The value g_elapsedSecondCount_simulationTime constantly changes as the simulation progresses, and so by doing something like:
	//	sin( g_elapsedSecondCount_simulationTime ) or cos( g_elapsedSecondCount_simulationTime )
	// you can get a value that will oscillate between [-1,1].
	// You should change at least one "channel" so that the color animates.
	// For example, to change red ("r") you would do something kind of like:
	//		o_color.r = ? sin( g_elapsedSecondCount_simulationTime ) ?
	// You can change .r, .g, and .b (but leave .a as 1.0).
	// Remember that your final values should be [0,1], so you will have to do some math.
}
