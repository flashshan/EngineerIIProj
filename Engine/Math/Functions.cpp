// Include Files
//==============

#include "Functions.h"

#include <cmath>

// Interface
//==========

float eae6320::Math::ConvertHorizontalFieldOfViewToVerticalFieldOfView( const float i_horizontalFieldOfView_inRadians,
	const float i_aspectRatio )
{
	return 2.0f * std::atan( std::tan( i_horizontalFieldOfView_inRadians * 0.5f ) / i_aspectRatio );
}
