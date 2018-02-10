// Include Files
//==============

#include "../cShaderBuilder.h"

#include <Engine/Platform/Platform.h>
#include <sstream>
#include <Tools/AssetBuildLibrary/Functions.h>

// Interface
//==========

// Build
//------

eae6320::cResult eae6320::Assets::cShaderBuilder::Build( const Graphics::ShaderTypes::eType i_shaderType, const std::vector<std::string>& i_arguments )
{
	auto result = Results::Success;

	// Create the command to run
	std::string command;
	{
		std::ostringstream commandToBuild;
		// Get the path to the shader compiler
		{
			std::string path_fxc;
			{
				// Get the path to the Windows SDK
				std::string path_sdk;
				{
					std::string errorMessage;
					if ( !( result = Platform::GetEnvironmentVariable( "WindowsSDKDir", path_sdk, &errorMessage ) ) )
					{
						OutputErrorMessageWithFileInfo( __FILE__, "Failed to get the path to the Windows SDK: %s", errorMessage.c_str() );
						goto OnExit;
					}
				}
				path_fxc = path_sdk + "/bin/" +
#ifndef _WIN64
					"x86"
#else
					"x64"
#endif
					+ "/fxc.exe";
			}
			commandToBuild << "\"" << path_fxc << "\"";
		}
		// Target profile
		switch ( i_shaderType )
		{
		case Graphics::ShaderTypes::Vertex:
			commandToBuild << " /Tvs_4_0";
			break;
		case Graphics::ShaderTypes::Fragment:
			commandToBuild << " /Tps_4_0";
			break;
		}
		// Get the content directories to use as #include search paths
		std::string EngineSourceContentDir, GameSourceContentDir;
		{
			std::string errorMessage;

			// EngineSourceContentDir
			if ( !( result = Platform::GetEnvironmentVariable( "EngineSourceContentDir", EngineSourceContentDir, &errorMessage ) ) )
			{
				OutputErrorMessage( "Failed to get the engine's source content directory: %s", errorMessage.c_str() );
				goto OnExit;
			}
			// GameSourceContentDir
			if ( !( result = Platform::GetEnvironmentVariable( "GameSourceContentDir", GameSourceContentDir, &errorMessage ) ) )
			{
				OutputErrorMessage( "Failed to get the game's source content directory: %s", errorMessage.c_str() );
				goto OnExit;
			}
		}
		// Entry point
		commandToBuild << " /Emain"
			// #define the platform
			<< " /DEAE6320_PLATFORM_D3D"
#ifdef EAE6320_GRAPHICS_AREDEBUGSHADERSENABLED
			// Disable optimizations so that debugging is easier
			<< " /Od"
			// Enable debugging
			<< " /Zi"
#endif
			// Target file
			<< " /Fo\"" << m_path_target << "\""
			// Additional #include paths
			// (list the game's content path before the engine's
			// so that if there are identically-named files
			// the game-specific one will be used)
			<< " /I\"" << GameSourceContentDir << ".\""	// FXC doesn't like the trailing slash, and so a dot is added
			<< " /I\"" << EngineSourceContentDir << ".\""
			// Don't output the logo
			<< " /nologo"
			// Source file
			<< " \"" << m_path_source << "\""
		;
		command = commandToBuild.str();
	}
	// Execute the command
	{
		int exitCode;
		std::string errorMessage;
		if ( result = Platform::ExecuteCommand( command.c_str(), &exitCode, &errorMessage ) )
		{
			result = ( exitCode == EXIT_SUCCESS ) ? Results::Success : Results::Failure;
		}
		else
		{
			OutputErrorMessageWithFileInfo( m_path_source, errorMessage.c_str() );
			goto OnExit;
		}
	}

OnExit:

	return result;
}
