// Include Files
//==============

#include "../cSprite.h"

#include "Includes.h"
#include "../cConstantBuffer.h"
#include "../cShader.h"
#include "../sContext.h"
#include "../VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Platform/Platform.h>

eae6320::cResult eae6320::Graphics::cSprite::Initialize(const eae6320::Graphics::VertexFormats::sSprite *i_vertexData, unsigned int i_vertexCount)
{
	auto result = eae6320::Results::Success;

	vertexCount_ = i_vertexCount;
	vertexData_ = new eae6320::Graphics::VertexFormats::sSprite[vertexCount_];
	memcpy(vertexData_, i_vertexData, sizeof(eae6320::Graphics::VertexFormats::sSprite) * i_vertexCount);

	// replace winding order	 // here is a simplest way to check winding order
	if (vertexCount_ > 2 && (vertexData_[0].x * vertexData_[1].y + vertexData_[1].x * vertexData_[2].y + vertexData_[2].x * vertexData_[0].y - vertexData_[0].y * vertexData_[1].x - vertexData_[1].y * vertexData_[2].x - vertexData_[2].y * vertexData_[0].x < 0.0f))
	{
		std::swap(vertexData_[1], vertexData_[2]);
	}

	// Create a vertex array object and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &s_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(s_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Create a vertex buffer object and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &s_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, s_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	// Assign the data to the buffer
	{
		const auto bufferSize = vertexCount_ * sizeof(eae6320::Graphics::VertexFormats::sSprite);
		EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(GLsizeiptr) * 8)));
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), reinterpret_cast<const GLvoid*>(vertexData_),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Initialize vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		const auto stride = static_cast<GLsizei>(sizeof(eae6320::Graphics::VertexFormats::sSprite));

		// Position (0)
		// 2 floats == 8 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			constexpr GLint elementCount = 2;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sSprite, x)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		{
			constexpr GLuint vertexElementLocation = 1;
			constexpr GLint elementCount = 2;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sSprite, u)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}

	}

OnExit:
	return result;
}


void eae6320::Graphics::cSprite::Draw()
{
	// Draw the geometry
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			glBindVertexArray(s_vertexArrayId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		// Render triangles from the currently-bound vertex buffer
		{
			// The mode defines how to interpret multiple vertices as a single "primitive";
			// a triangle list is defined
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			constexpr GLenum mode = GL_TRIANGLE_STRIP;
			// It's possible to start rendering primitives in the middle of the stream
			constexpr GLint indexOfFirstVertexToRender = 0;
			// As of this comment we are only drawing a single triangle
			// (you will have to update this code in future assignments!)

			glDrawArrays(mode, indexOfFirstVertexToRender, vertexCount_);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
	}
}

eae6320::cResult eae6320::Graphics::cSprite::CleanUp()
{
	auto result = Results::Success;

	{
		if (s_vertexArrayId != 0)
		{
			// Make sure that the vertex array isn't bound
			{
				// Unbind the vertex array
				glBindVertexArray(0);
				const auto errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					if (result)
					{
						result = Results::Failure;
					}
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					Logging::OutputError("OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
			}
			constexpr GLsizei arrayCount = 1;
			glDeleteVertexArrays(arrayCount, &s_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_vertexArrayId = 0;
		}
		if (s_vertexBufferId != 0)
		{
			constexpr GLsizei bufferCount = 1;
			glDeleteBuffers(bufferCount, &s_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_vertexBufferId = 0;
		}
	}

	return result;
}