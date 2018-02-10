// Include Files
//==============

#include "cRenderState.h"

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cRenderState::Initialize( const uint8_t i_renderStateBits )
{
	m_bits = i_renderStateBits;
	return InitializeFromBits();
}

eae6320::Graphics::cRenderState::~cRenderState()
{
	CleanUp();
}
