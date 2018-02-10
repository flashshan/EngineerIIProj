// Include Files
//==============

#include "../cView.h"
#include "Includes.h"
#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>


void eae6320::Graphics::cView::ClearView(float i_r, float i_g, float i_b, float i_a)
{
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		// Black is usually used
		{
			glClearColor(i_r, i_g, i_b, i_a);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		{
			constexpr GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
			glClear(clearColor);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
	}
}

void eae6320::Graphics::cView::ClearDepth(float i_depth)
{
	{
		glDepthMask(GL_TRUE);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		glClearDepth(i_depth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	{
		constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
		glClear(clearDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}


void eae6320::Graphics::cView::SwapBuffer()
{
	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer")
	{
		const auto deviceContext = sContext::g_context.deviceContext;
		EAE6320_ASSERT(deviceContext != NULL);

		const auto glResult = SwapBuffers(deviceContext);
		EAE6320_ASSERT(glResult != FALSE);
	}
}

eae6320::cResult eae6320::Graphics::cView::Initialize(const sInitializationParameters& i_initializationParameters)
{
	return eae6320::Results::Success;
}

void eae6320::Graphics::cView::CleanUp()
{
}
