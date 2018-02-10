#ifndef EAE6320_GRAPHICS_CEFFECT_H
#define EAE6320_GRAPHICS_CEFFECT_H

// Include Files
//==============

#include "cShader.h"
#include "cRenderState.h"

#ifdef EAE6320_PLATFORM_GL
	#include "OpenGL/Includes.h"
#endif


#ifdef EAE6320_PLATFORM_D3D
	struct ID3D11InputLayout;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cEffect
		{
		public:
			// hide illegal functions
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect);

			// Reference Counting
			//-------------------
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// default factory function
			static cResult MakeEffect(cEffect*& o_effect, const char *i_vertShaderFile, const char *i_fragShaderFile, uint8_t i_renderState = RenderStates::NoEffect);

			// interface for user to manually create effect
			static cResult Load(cEffect*& o_effect, const char *i_vertShaderFile, const char *i_fragShaderFile, uint8_t i_renderState);	

			// renderState can not be changed at runtime
			cResult Initialize(const char *i_vertShaderFile, const char *i_fragShaderFile, uint8_t i_renderState);

			void Bind();
			cResult CleanUp();

		private:
			cEffect() {};							// hide constructor
			~cEffect() { this->CleanUp(); };        // do clean up

			// Shading Data
			//-------------
			eae6320::Graphics::cShader::Handle s_vertexShader;
			eae6320::Graphics::cShader::Handle s_fragmentShader;

#ifdef EAE6320_PLATFORM_GL
			GLuint s_programId = 0;
#endif
			// declare counting
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			eae6320::Graphics::cRenderState s_renderState;
		};
		
		

	}
}

#endif	// EAE6320_GRAPHICS_CSHADER_H
