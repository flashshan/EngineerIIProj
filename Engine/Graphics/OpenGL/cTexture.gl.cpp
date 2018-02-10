// Include Files
//==============

#include "../cTexture.h"

#include <algorithm>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Helper Function Declarations
//=============================

namespace
{
	constexpr GLenum GetGlFormat( const eae6320::Graphics::TextureFormats::Compression::eType i_compressionType );
}

// Interface
//==========

// Render
//-------

void eae6320::Graphics::cTexture::Bind( const unsigned int i_id ) const
{
	// Make the texture unit active
	{
		glActiveTexture( GL_TEXTURE0 + static_cast<GLint>( i_id ) );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	// Bind the texture to the texture unit
	{
		EAE6320_ASSERT( m_textureId != 0 );
		glBindTexture( GL_TEXTURE_2D, m_textureId );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
}

// Initialization / Clean Up
//--------------------------

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cTexture::Initialize( const char* const i_path, const void* const i_textureData, const size_t i_textureDataSize )
{
	auto result = Results::Success;

	// Create a new texture and make it active
	{
		constexpr GLsizei textureCount = 1;
		glGenTextures( textureCount, &m_textureId );
		const auto errorCode = glGetError();
		if ( errorCode == GL_NO_ERROR )
		{
			if ( m_textureId != 0 )
			{
				glBindTexture( GL_TEXTURE_2D, m_textureId );
				const auto errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to bind the new texture %u for %s: %s",
						m_textureId, i_path, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			else
			{
				result = Results::Failure;
				EAE6320_ASSERT( false );
				eae6320::Logging::OutputError( "OpenGL failed to create a texture for %s", i_path );
				goto OnExit;
			}
		}
		else
		{
			result = Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to create a texture for %s: %s",
				i_path, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			goto OnExit;
		}
	}
	// Fill in the data for each MIP level
	{
		auto currentWidth = static_cast<GLsizei>( m_info.width );
		auto currentHeight = static_cast<GLsizei>( m_info.height );
		auto currentOffset = reinterpret_cast<uintptr_t>( i_textureData );
		const auto finalOffset = currentOffset + i_textureDataSize;
		const auto compressionType = m_info.compressionType;
		const auto blockSize = TextureFormats::Compression::GetSizeOfBlock( compressionType );
		const auto glFormat = GetGlFormat( compressionType );
		constexpr GLint borderWidth = 0;
		const auto mipMapCount = static_cast<GLint>( m_info.mipMapCount );
		for ( GLint i = 0; i < mipMapCount; ++i )
		{
			// Calculate how much memory this MIP level uses
			const auto blockCount_singleRow = ( currentWidth + 3 ) / 4;
			const auto byteCount_singleRow = blockCount_singleRow * blockSize;
			const auto rowCount = ( currentHeight + 3 ) / 4;
			const auto byteCount_currentMipLevel = byteCount_singleRow * rowCount;
			// Set the data into the texture
			glCompressedTexImage2D( GL_TEXTURE_2D, i, glFormat, currentWidth, currentHeight,
				borderWidth, byteCount_currentMipLevel, reinterpret_cast<void*>( currentOffset ) );
			const auto errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				// Update current data for next iteration
				{
					currentOffset += byteCount_currentMipLevel;
					if ( currentOffset <= finalOffset )
					{
						currentWidth = std::max( currentWidth / 2, 1 );
						currentHeight = std::max( currentHeight / 2, 1 );
					}
					else
					{
						result = Results::InvalidFile;
						EAE6320_ASSERTF( false, "Texture file %s is too small to contain MIP map #%i",
							i_path, i );
						Logging::OutputError( "The texture file %s is too small to contain MIP map #%i",
							i_path, i );
						goto OnExit;
					}
				}
			}
			else
			{
				result = Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to copy the texture data from MIP map #%i of %s: %s",
					i, i_path, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		EAE6320_ASSERTF( currentOffset == finalOffset, "The texture file %s has more texture data (%u) than it should (%u)",
			i_path, finalOffset, currentOffset );
	}

OnExit:

	if ( !result && ( m_textureId != 0 ) )
	{
		constexpr GLsizei textureCount = 1;
		glDeleteTextures( textureCount, &m_textureId );
		EAE6320_ASSERT( glGetError == GL_NO_ERROR );
		m_textureId = 0;
	}
	
	return result;
}

eae6320::cResult eae6320::Graphics::cTexture::CleanUp()
{
	auto result = Results::Success;

	if ( m_textureId != 0 )
	{
		constexpr GLsizei textureCount = 1;
		glDeleteTextures( textureCount, &m_textureId );
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			if ( result )
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the texture %u: %s",
				m_textureId, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		m_textureId = 0;
	}

	return result;
}

// Helper Function Definitions
//============================

namespace
{
	constexpr GLenum GetGlFormat( const eae6320::Graphics::TextureFormats::Compression::eType i_compressionType )
	{
		switch ( i_compressionType )
		{
			case eae6320::Graphics::TextureFormats::Compression::BC1: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			case eae6320::Graphics::TextureFormats::Compression::BC3: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}

		// Other formats are possible, but not for our class
		EAE6320_ASSERT( false );
		return 0;
	}
}
