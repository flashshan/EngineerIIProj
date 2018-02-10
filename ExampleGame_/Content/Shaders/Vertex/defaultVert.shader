#include <Shaders/shaders.inc>


// Entry Point
//============

#if defined( EAE6320_PLATFORM_D3D )

void main(

	// Input
	//=====
	in const float2 i_position : POSITION,
	in const float2 i_uv : TEXCOORD0,

	// Output
	//=======
	out float4 o_position : SV_POSITION,
	out float2 o_uv : TEXCOORD0
)

#elif defined( EAE6320_PLATFORM_GL )

// Input
//======
layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_uv;

// Output
//=======
layout(location = 0) out vec2 o_uv;

// Entry Point
//============
void main()

#endif
// Calculate the position of this vertex on screen
{
	// This example shader sets the "out" position directly from the "in" position:
	o_position = float4(i_position.x, i_position.y, 0.0, 1.0);
	o_uv = i_uv;

	// rotation triangle
	/*o_position.x = i_position.x * cos(g_elapsedSecondCount_simulationTime) + i_position.y * sin(g_elapsedSecondCount_simulationTime);
	o_position.y = i_position.y * cos(g_elapsedSecondCount_simulationTime) - i_position.x * sin(g_elapsedSecondCount_simulationTime);*/

	// softly deforming triangle
	/*o_position.x = sin(i_position.y + g_elapsedSecondCount_simulationTime) * 0.2 + i_position.x;
	o_position.y = sin(i_position.x + g_elapsedSecondCount_simulationTime) * 0.2 + i_position.y;*/
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
