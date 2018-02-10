/*
	Sampler state determines how textures are sampled in shaders
*/

#ifndef EAE6320_GRAPHICS_CSAMPLERSTATE_H
#define EAE6320_GRAPHICS_CSAMPLERSTATE_H

// Include Files
//==============

#include "Configuration.h"

#include <Engine/Results/Results.h>

#if defined( EAE6320_PLATFORM_GL )
	#include "OpenGL/Includes.h"
#endif

// Forward Declarations
//=====================

#if defined( EAE6320_PLATFORM_D3D )
	struct ID3D11SamplerState;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cSamplerState
		{
			// Interface
			//==========

		public:

			// Render
			//-------

			void Bind() const;

			// Initialization / Clean Up
			//--------------------------

			cResult Initialize();
			cResult CleanUp();

			cSamplerState() = default;
			~cSamplerState();

			// Data
			//=====

		private:

#if defined( EAE6320_PLATFORM_D3D )
			ID3D11SamplerState* m_samplerState = nullptr;
#else
			GLuint m_samplerStateId = 0;
#endif

		};
	}
}

#endif	// EAE6320_GRAPHICS_CSAMPLERSTATE_H
