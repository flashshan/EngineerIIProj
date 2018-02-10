// Include Files
//==============

#include "../cSamplerState.h"

#include "Includes.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Interface
//==========

// Render
//-------

void eae6320::Graphics::cSamplerState::Bind() const
{
	// The register must match the ID defined in the shader;
	// In our class we will only have a single sampler state
	constexpr GLuint textureUnitId = 0;
	EAE6320_ASSERT( m_samplerStateId != 0 );
	glBindSampler( textureUnitId, m_samplerStateId );
	EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cSamplerState::Initialize()
{
	auto result = Results::Success;

	constexpr GLsizei samplerStateCount = 1;
	glGenSamplers( samplerStateCount, &m_samplerStateId );
	const auto errorCode = glGetError();
	if ( errorCode == GL_NO_ERROR )
	{
		if ( m_samplerStateId != 0 )
		{
			// Linear Filtering
			glSamplerParameteri( m_samplerStateId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			{
				const auto errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to set the sampler state's minifying filter: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			glSamplerParameteri( m_samplerStateId, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			{
				const auto errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to set the sampler state's magnifying filter: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			// If UVs go outside [0,1] wrap them around (so that textures can tile)
			glSamplerParameteri( m_samplerStateId, GL_TEXTURE_WRAP_S, GL_REPEAT );
			{
				const auto errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to set the sampler state's horizontal wrapping behavior: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			glSamplerParameteri( m_samplerStateId, GL_TEXTURE_WRAP_T, GL_REPEAT );
			{
				const auto errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to set the sampler state's vertical wrapping behavior: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERT( false );
			eae6320::Logging::OutputError( "OpenGL failed to create a sampler state" );
			goto OnExit;
		}
	}
	else
	{
		result = eae6320::Results::Failure;
		EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		eae6320::Logging::OutputError( "OpenGL failed to create a sampler state: %s",
			reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		goto OnExit;
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cSamplerState::CleanUp()
{
	auto result = Results::Success;

	if ( m_samplerStateId != 0 )
	{
		constexpr GLsizei samplerStateCount = 1;
		glDeleteSamplers( samplerStateCount, &m_samplerStateId );
		m_samplerStateId = 0;
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			if ( result )
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to delete the sampler state: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
	}

	return result;
}
