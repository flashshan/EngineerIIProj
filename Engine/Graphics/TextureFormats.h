/*
	A texture format determines the layout of the texture data
	that the CPU will send to the GPU
*/

#ifndef EAE6320_GRAPHICS_TEXTUREFORMATS_H
#define EAE6320_GRAPHICS_TEXTUREFORMATS_H

// Include Files
//==============

#include "Configuration.h"

#include <cstdint>

// Texture Formats
//================

namespace eae6320
{
	namespace Graphics
	{
		namespace TextureFormats
		{
			// Most graphics hardware natively supports a specific kind of texture compression
			// (formerly known as "DXT" or "S3TC", but now more commonly called "BC" for "block compression").
			// This isn't a particularly efficient compression scheme
			// (compared to the more common image formats you may know),
			// but since it can be decompressed for free by graphics hardware
			// it is a good choice for games.
			namespace Compression
			{
				enum eType : uint8_t
				{
					BC1,
					BC2,
					BC3,
					BC4,
					BC5,
					BC7,

					Unknown
				};

				// Returns the size in bytes of a single 4x4 (16 pixel) block
				inline constexpr unsigned int GetSizeOfBlock( const eType i_format )
				{
					switch ( i_format )
					{
						// RGB (with an optional binary alpha)
						case BC1: return 8;
						// RGB + A (BC1 for RGB and 4 bits for each alpha pixel)
						case BC2: return GetSizeOfBlock( BC1 ) + ( 4 * 16 );
						// RGB + A (BC1 for RGB and BC4 for alpha)
						case BC3: return GetSizeOfBlock( BC1 ) + GetSizeOfBlock( BC4 );
						// Single channel
						case BC4: return 8;
						// Two channels (2 BC4s)
						case BC5: return GetSizeOfBlock( BC4 ) + GetSizeOfBlock( BC4 );
						// RGBA
						case BC7: return 16;

					// The format doesn't use block compression
					default:
						return 0;
					}
				}
			}

			// This struct is a binary description of the texture that is stored in a texture file
			// and loaded and used at run-time
			struct sTextureInfo
			{
				uint16_t width, height;
				uint8_t mipMapCount;
				Compression::eType compressionType;
			};
		}
	}
}

#endif	//EAE6320_GRAPHICS_TEXTUREFORMATS_H
