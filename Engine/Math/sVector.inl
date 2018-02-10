#ifndef EAE6320_MATH_SVECTOR_INL
#define EAE6320_MATH_SVECTOR_INL

// Include Files
//==============

#include "sVector.h"

// Interface
//==========

// Multiplication
//---------------

inline eae6320::Math::sVector operator *( const float i_lhs, const eae6320::Math::sVector& i_rhs )
{
	return i_rhs * i_lhs;
}

// Initialization / Shut Down
//---------------------------

inline eae6320::Math::sVector::sVector( const float i_x, const float i_y, const float i_z )
	:
	x( i_x ), y( i_y ), z( i_z )
{

}

#endif	// EAE6320_MATH_SVECTOR_INL
