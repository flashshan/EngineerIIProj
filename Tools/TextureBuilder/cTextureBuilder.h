/*
	This class builds hardware-ready textures from source images
*/

#ifndef EAE6320_CTEXTUREBUILDER_H
#define EAE6320_CTEXTUREBUILDER_H

// Include Files
//==============

#include <Tools/AssetBuildLibrary/cbBuilder.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cTextureBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			virtual cResult Build( const std::vector<std::string>& i_arguments ) override;
		};
	}
}

#endif	// EAE6320_CTEXTUREBUILDER_H
