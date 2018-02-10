/*
	A texture is an n-dimensional array of data
	(usually a 2-dimensional array of colors)
	that can be sampled in a shader
*/

#ifndef EAE6320_GRAPHICS_CTEXTURE_H
#define EAE6320_GRAPHICS_CTEXTURE_H

// Include Files
//==============

#include "Configuration.h"

#include <Engine/Assets/ReferenceCountedAssets.h>

#include "TextureFormats.h"

#include <cstdint>
#include <Engine/Assets/cHandle.h>
#include <Engine/Assets/cManager.h>
#include <Engine/Results/Results.h>

#ifdef EAE6320_PLATFORM_GL
	#include "OpenGL/Includes.h"
#endif

// Forward Declarations
//=====================

#ifdef EAE6320_PLATFORM_D3D
	struct ID3D11ShaderResourceView;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cTexture
		{
			// Interface
			//==========

		public:

			// Render
			//-------

			// The ID is the HLSL register or GLSL layout ID
			// defined in the shader that uses this texture.
			// The texture doesn't know what this is
			// (a single texture could be used by many different effects)
			// and so this is the caller's responsibility to pass in.
			void Bind( const unsigned int i_id ) const;

			// Access
			//-------

			using Handle = Assets::cHandle<cTexture>;
			static Assets::cManager<cTexture> s_manager;

			uint16_t GetWidth() const;
			uint16_t GetHeight() const;

			// Initialization / Clean Up
			//--------------------------

			static cResult Load( const char* const i_path, cTexture*& o_texture );

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS( cTexture );

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

		private:

#if defined( EAE6320_PLATFORM_D3D )
			ID3D11ShaderResourceView* m_textureView = nullptr;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_textureId = 0;
#endif

			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			TextureFormats::sTextureInfo m_info;

			// Implementation
			//===============

		private:

			// Initialization / Clean Up
			//--------------------------

			cResult Initialize( const char* const i_path, const void* const i_textureData, const size_t i_textureDataSize );
			cResult CleanUp();

			cTexture( const TextureFormats::sTextureInfo& i_info );
			~cTexture();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CTEXTURE_H
