/*
	A vertex format determines the layout of the geometric data
	that the CPU will send to the GPU
*/

#ifndef EAE6320_GRAPHICS_VERTEXFORMATS_H
#define EAE6320_GRAPHICS_VERTEXFORMATS_H

// Include Files
//==============

#include "Configuration.h"

// Vertex Formats
//===============

namespace eae6320
{
	namespace Graphics
	{
		namespace VertexFormats
		{
			struct sGeometry
			{
				float x, y, z;
				uint8_t r, g, b, a;
				float u, v;
			};

			struct sSprite
			{
				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				float x, y;
				float u, v;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_VERTEXFORMATS_H
