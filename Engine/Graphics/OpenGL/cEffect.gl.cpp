// Include Files
//==============

#include "../cEffect.h"

#include "Includes.h"
#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>


void eae6320::Graphics::cEffect::Bind()
{
	// Bind the shading data
	{
		{
			EAE6320_ASSERT(s_programId != 0);
			glUseProgram(s_programId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		s_renderState.Bind();
	}
}

