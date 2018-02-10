#include "cSprite.h"

#include <Engine//Logging/Logging.h>
#include <new>

eae6320::cResult eae6320::Graphics::cSprite::MakeSprite(cSprite*& o_sprite, const VertexFormats::sSprite *i_vertexData, unsigned int i_vertexCount)
{
	auto result = eae6320::Results::Success;

	eae6320::Graphics::cSprite* newSprite = new (std::nothrow) eae6320::Graphics::cSprite();

	// allocate
	if (!newSprite)
	{
		result = Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for the sprite");
		Logging::OutputError("Failed to allocate memory for the sprite");
		goto OnExit;
	}


	// init
	if (!(result = newSprite->Initialize(i_vertexData, i_vertexCount)))
	{
		EAE6320_ASSERTF(false, "Initialization of new sprite failed");
		goto OnExit;
	}

OnExit:

	if (result)
	{
		EAE6320_ASSERT(newSprite);
		o_sprite = newSprite;
	}
	else
	{
		if (newSprite)
		{
			newSprite->DecrementReferenceCount();
			newSprite = nullptr;
		}
		o_sprite = nullptr;
	}
	return result;
}

eae6320::cResult eae6320::Graphics::cSprite::Load(cSprite*& o_sprite, const VertexFormats::sSprite *i_vertexData, unsigned int i_vertexCount)
{
	return eae6320::Results::Success;
}


eae6320::Graphics::cSprite::~cSprite()     // do clean up
{
	this->CleanUp();
	delete[] vertexData_;
};