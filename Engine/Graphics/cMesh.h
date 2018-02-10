#ifndef EAE6320_GRAPHICS_CMESH_H
#define EAE6320_GRAPHICS_CMESH_H

// Include Files
//==============

//#include "Configuration.h"

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <Engine/Assets/cHandle.h>
#include <Engine/Assets/cManager.h>
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
		class cMesh
		{
		public:

			// manager
			using Handle = Assets::cHandle<cMesh>;
			static Assets::cManager<cMesh> s_manager;

			// default factory function
			static cResult MakeMesh(cMesh*& o_mesh, const char* const i_meshFilePath);

			// interface for user to manually create mesh
			static cResult Load(const char* const i_meshFilePath, cMesh*& o_mesh);

			cResult Initialize(const VertexFormats::sGeometry *i_vertexData, unsigned int i_vertexCount, const uint16_t *i_indexData, unsigned int i_indexCount);
			void Draw();
			cResult CleanUp();

			// hide illegal functions
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cMesh);

			// Reference Counting
			//-------------------
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

		private:
			cMesh() {};
			~cMesh();     // do clean up

			// Geometry Data
			//-------------- 
#ifdef EAE6320_PLATFORM_D3D
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* s_vertexBuffer = nullptr;
			ID3D11Buffer* s_indexBuffer = nullptr;
			// D3D has an "input layout" object that associates the layout of the vertex format struct
			// with the input from a vertex shader
			ID3D11InputLayout* s_vertexInputLayout = nullptr;
#elif defined EAE6320_PLATFORM_GL
			// A vertex buffer holds the data for each vertex
			GLuint s_vertexBufferId = 0;
			GLuint s_indexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint s_vertexArrayId = 0;
#endif
			unsigned int indexCount_;

			// declare counting
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CSPRITE_H
