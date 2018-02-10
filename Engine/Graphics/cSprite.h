#ifndef EAE6320_GRAPHICS_CSPRITE_H
#define EAE6320_GRAPHICS_CSPRITE_H

// Include Files
//==============

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <Engine/Results/Results.h>
#include "VertexFormats.h"


#ifdef EAE6320_PLATFORM_GL
	#include "OpenGL/Includes.h"
#endif

// Forward Declarations
//=====================

#ifdef EAE6320_PLATFORM_D3D
	struct ID3D11Buffer;
	struct ID3D11InputLayout;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cSprite
		{
		public:
			// hide illegal functions
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cSprite);

			// Reference Counting
			//-------------------
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();
			
			// default factory function
			static cResult MakeSprite(cSprite*& o_sprite, const VertexFormats::sSprite *i_vertexData, unsigned int i_vertexCount);

			// interface for user to manually create sprite
			static cResult Load(cSprite*& o_sprite, const VertexFormats::sSprite *i_vertexData, unsigned int i_vertexCount);

			cResult Initialize(const VertexFormats::sSprite *i_vertexData, unsigned int i_vertexCount);
			void Draw();
			cResult CleanUp();

		private:
			cSprite() {};
			~cSprite();     // do clean up

			VertexFormats::sSprite *vertexData_;

			// Geometry Data
			//-------------- 
#ifdef EAE6320_PLATFORM_D3D
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* s_vertexBuffer = nullptr;
			// D3D has an "input layout" object that associates the layout of the vertex format struct
			// with the input from a vertex shader
			ID3D11InputLayout* s_vertexInputLayout = nullptr;
#elif defined EAE6320_PLATFORM_GL
			// A vertex buffer holds the data for each vertex
			GLuint s_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint s_vertexArrayId = 0;
#endif
			unsigned int vertexCount_;

			// declare counting
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CSPRITE_H
