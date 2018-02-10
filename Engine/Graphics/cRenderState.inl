#ifndef EAE6320_GRAPHICS_CRENDERSTATE_INL
#define EAE6320_GRAPHICS_CRENDERSTATE_INL

// Include Files
//==============

#include "cRenderState.h"

// Interface
//==========

// Alpha Transparency
inline bool eae6320::Graphics::RenderStates::IsAlphaTransparencyEnabled( const uint8_t i_renderStateBits )
{
	return ( i_renderStateBits & AlphaTransparency ) != 0;
}
inline void eae6320::Graphics::RenderStates::EnableAlphaTransparency( uint8_t& io_renderStateBits )
{
	io_renderStateBits |= AlphaTransparency;
}
inline void eae6320::Graphics::RenderStates::DisableAlphaTransparency( uint8_t& io_renderStateBits )
{
	io_renderStateBits &= ~AlphaTransparency;
}

// Depth Buffering
inline bool eae6320::Graphics::RenderStates::IsDepthBufferingEnabled( const uint8_t i_renderStateBits )
{
	return ( i_renderStateBits & DepthBuffering ) != 0;
}
inline void eae6320::Graphics::RenderStates::EnableDepthBuffering( uint8_t& io_renderStateBits )
{
	io_renderStateBits |= DepthBuffering;
}
inline void eae6320::Graphics::RenderStates::DisableDepthBuffering( uint8_t& io_renderStateBits )
{
	io_renderStateBits &= ~DepthBuffering;
}

// Draw Both Triangle Sides
inline bool eae6320::Graphics::RenderStates::ShouldBothTriangleSidesBeDrawn( const uint8_t i_renderStateBits )
{
	return ( i_renderStateBits & DrawBothTriangleSides ) != 0;
}
inline void eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides( uint8_t& io_renderStateBits )
{
	io_renderStateBits |= DrawBothTriangleSides;
}
inline void eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides( uint8_t& io_renderStateBits )
{
	io_renderStateBits &= ~DrawBothTriangleSides;
}

// Access
//-------

inline bool eae6320::Graphics::cRenderState::IsAlphaTransparencyEnabled() const
{
	return RenderStates::IsAlphaTransparencyEnabled( m_bits );
}
inline bool eae6320::Graphics::cRenderState::IsDepthBufferingEnabled() const
{
	return RenderStates::IsDepthBufferingEnabled( m_bits );
}
inline bool eae6320::Graphics::cRenderState::ShouldBothTriangleSidesBeDrawn() const
{
	return RenderStates::ShouldBothTriangleSidesBeDrawn( m_bits );
}


inline uint8_t eae6320::Graphics::cRenderState::GetRenderStateBits() const
{
	return m_bits;
}


#endif	// EAE6320_GRAPHICS_CRENDERSTATE_INL
