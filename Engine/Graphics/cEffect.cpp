#include "cEffect.h"

eae6320::cResult eae6320::Graphics::cEffect::MakeEffect(eae6320::Graphics::cEffect*& o_effect, const char *i_vertShaderFile, const char *i_fragShaderFile, uint8_t i_renderState)
{
	auto result = eae6320::Results::Success;

	eae6320::Graphics::cEffect* newEffect = new (std::nothrow) eae6320::Graphics::cEffect();

	// allocate
	if (!newEffect)
	{
		result = Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for the effect");
		Logging::OutputError("Failed to allocate memory for the effect");
		goto OnExit;
	}


	// init
	if (!(result = newEffect->Initialize(i_vertShaderFile, i_fragShaderFile, i_renderState)))
	{
		EAE6320_ASSERTF(false, "Initialization of new effect failed");
		goto OnExit;
	}

OnExit:

	if (result)
	{
		EAE6320_ASSERT(newEffect);
		o_effect = newEffect;
	}
	else
	{
		if (newEffect)
		{
			newEffect->DecrementReferenceCount();
			newEffect = nullptr;
		}
		o_effect = nullptr;
	}
	return result;
}

// currently not being used
eae6320::cResult eae6320::Graphics::cEffect::Load(eae6320::Graphics::cEffect*& o_effect, const char *i_vertShaderFile, const char *i_fragShaderFile, uint8_t i_renderState)
{
	return eae6320::Results::Success;
}

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const char *i_vertShaderFile, const char *i_fragShaderFile, uint8_t i_renderState)
{
	auto result = eae6320::Results::Success;

	// init vertex shader
	if ((!(result = eae6320::Graphics::cShader::s_manager.Load(i_vertShaderFile, s_vertexShader, eae6320::Graphics::ShaderTypes::Vertex))) &&
		(!(result = eae6320::Graphics::cShader::s_manager.Load("data/Shaders/Vertex/defaultVert.shd", s_vertexShader, eae6320::Graphics::ShaderTypes::Vertex))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	// init fragment shader
	if ((!(result = eae6320::Graphics::cShader::s_manager.Load(i_fragShaderFile, s_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment))) &&
		(!(result = eae6320::Graphics::cShader::s_manager.Load("data/Shaders/Fragment/defaultFrag.shd", s_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	// init render state
	if ((!(result = s_renderState.Initialize(i_renderState))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

#ifdef EAE6320_PLATFORM_D3D
	OnExit:

#elif defined EAE6320_PLATFORM_GL
	// Create a program
	{
		s_programId = glCreateProgram();
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to create a program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
		else if (s_programId == 0)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("OpenGL failed to create a program");
			goto OnExit;
		}
	}
	// Attach the shaders to the program
	{
		// Vertex
		{
			glAttachShader(s_programId, eae6320::Graphics::cShader::s_manager.Get(s_vertexShader)->m_shaderId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to attach the vertex shader to the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		// Fragment
		{
			glAttachShader(s_programId, eae6320::Graphics::cShader::s_manager.Get(s_fragmentShader)->m_shaderId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to attach the fragment shader to the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
	}
	// Link the program
	{
		glLinkProgram(s_programId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get link info
			// (this won't be used unless linking fails
			// but it can be useful to look at when debugging)
			std::string linkInfo;
			{
				GLint infoSize;
				glGetProgramiv(s_programId, GL_INFO_LOG_LENGTH, &infoSize);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					struct sLogInfo
					{
						GLchar* memory;
						sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
						~sLogInfo() { if (memory) free(memory); }
					} info(static_cast<size_t>(infoSize));
					GLsizei* const dontReturnLength = nullptr;
					glGetProgramInfoLog(s_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					const auto errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						linkInfo = info.memory;
					}
					else
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			// Check to see if there were link errors
			GLint didLinkingSucceed;
			{
				glGetProgramiv(s_programId, GL_LINK_STATUS, &didLinkingSucceed);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didLinkingSucceed == GL_FALSE)
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, linkInfo.c_str());
						eae6320::Logging::OutputError("The program failed to link: %s",
							linkInfo.c_str());
						goto OnExit;
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

OnExit:
	if (!result)
	{
		if (s_programId != 0)
		{
			glDeleteProgram(s_programId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_programId = 0;
		}
	}
#endif

	return result;
}



eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	// clean up
	auto result = eae6320::Results::Success;

#ifdef EAE6320_PLATFORM_GL
	if (s_programId != 0)
	{
		glDeleteProgram(s_programId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_programId = 0;
	}
#endif

	if (s_vertexShader)
	{
		const auto localResult = cShader::s_manager.Release(s_vertexShader);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	if (s_fragmentShader)
	{
		const auto localResult = cShader::s_manager.Release(s_fragmentShader);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = s_renderState.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	return result;
}
