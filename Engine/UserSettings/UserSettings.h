/*
	This file manages user-configurable settings
	(ones that the user can set in an INI file)
*/

#ifndef EAE6320_USERSETTINGS_H
#define EAE6320_USERSETTINGS_H

// Include Files
//==============

#include <cstdint>
#include <Engine/Results/Results.h>

// Interface
//==========

namespace eae6320
{
	namespace UserSettings
	{
		cResult GetDesiredInitialResolutionWidth( uint16_t& o_width );
		cResult GetDesiredInitialResolutionHeight( uint16_t& o_height );
	}
}

#endif	// EAE6320_USERSETTINGS_H
