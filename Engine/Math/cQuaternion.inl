#ifndef EAE6320_MATH_CQUATERNION_INL
#define EAE6320_MATH_CQUATERNION_INL

// Include Files
//==============

#include "cQuaternion.h"

// Implementation
//===============

// Initialization / Shut Down
//---------------------------

inline eae6320::Math::cQuaternion::cQuaternion( const float i_w, const float i_x, const float i_y, const float i_z )
	:
	m_w( i_w ), m_x( i_x ), m_y( i_y ), m_z( i_z )
{

}

#endif	// EAE6320_MATH_CQUATERNION_INL
