// Include Files
//==============

#include "UserSettings.h"

#include <cmath>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <External/Lua/Includes.h>
#include <string>

// Static Data Initialization
//===========================

namespace
{
	uint16_t s_resolutionHeight = 0;
	auto s_resolutionHeight_validity = eae6320::Results::Failure;
	uint16_t s_resolutionWidth = 0;
	auto s_resolutionWidth_validity = eae6320::Results::Failure;

	constexpr auto* const s_userSettingsFileName = "settings.ini";
}

// Helper Function Declarations
//=============================

namespace
{
	eae6320::cResult InitializeIfNecessary();
	eae6320::cResult LoadUserSettingsIntoLuaTable( lua_State& io_luaState );
	eae6320::cResult PopulateUserSettingsFromLuaTable( lua_State& io_luaState );
}

// Interface
//==========

eae6320::cResult eae6320::UserSettings::GetDesiredInitialResolutionWidth( uint16_t& o_width )
{
	const auto result = InitializeIfNecessary();
	if ( result )
	{
		if ( s_resolutionWidth_validity )
		{
			o_width = s_resolutionWidth;
		}
		return s_resolutionWidth_validity;
	}
	else
	{
		return result;
	}
}

eae6320::cResult eae6320::UserSettings::GetDesiredInitialResolutionHeight( uint16_t& o_height )
{
	const auto result = InitializeIfNecessary();
	if ( result )
	{
		if ( s_resolutionHeight_validity )
		{
			o_height = s_resolutionHeight;
		}
		return s_resolutionHeight_validity;
	}
	else
	{
		return result;
	}
}

// Helper Function Definitions
//============================

namespace
{
	eae6320::cResult InitializeIfNecessary()
	{
		static eae6320::cResult isInitialized;
		if ( isInitialized != eae6320::Results::Undefined )
		{
			return isInitialized;
		}

		auto result = eae6320::Results::Success;

		// Create a new Lua state
		lua_State* luaState = nullptr;
		auto wasUserSettingsEnvironmentCreated = false;
		{
			luaState = luaL_newstate();
			if ( !luaState )
			{
				result = eae6320::Results::OutOfMemory;
				EAE6320_ASSERTF( false, "Failed to create a new Lua state" );
				eae6320::Logging::OutputError( "Failed to create a new Lua state for the user settings" );
				goto CleanUp;
			}
		}
		// Create an empty table to be used as the Lua environment for the user settings
		{
			lua_newtable( luaState );
			wasUserSettingsEnvironmentCreated = true;
		}
		// Load the user settings
		if ( result = LoadUserSettingsIntoLuaTable( *luaState ) )
		{
			// Populate the user settings in C from the user settings in the Lua environment
			if ( !( result = PopulateUserSettingsFromLuaTable( *luaState ) ) )
			{
				goto CleanUp;
			}
		}
		else
		{
			goto CleanUp;
		}

		// Free the Lua environment

	CleanUp:

		if ( luaState )
		{
			if ( wasUserSettingsEnvironmentCreated )
			{
				lua_pop( luaState, 1 );
			}
			EAE6320_ASSERTF( lua_gettop( luaState ) == 0, "Lua stack is inconsistent" );
			lua_close( luaState );
		}

		isInitialized = result;
		return isInitialized;
	}

	eae6320::cResult LoadUserSettingsIntoLuaTable( lua_State& io_luaState )
	{
		// Load the file into a Lua function
		if ( eae6320::Platform::DoesFileExist( s_userSettingsFileName ) )
		{
			const auto luaResult = luaL_loadfile( &io_luaState, s_userSettingsFileName );
			if ( luaResult == LUA_OK )
			{
				// Set the Lua function's environment
				{
					lua_pushvalue( &io_luaState, -2 );
					const auto* const upValueName = lua_setupvalue( &io_luaState, -2, 1 );
					if ( !upValueName )
					{
						EAE6320_ASSERT( false );
						eae6320::Logging::OutputError( "Internal error setting the Lua environment for the user settings file \"%s\"!"
							" This should never happen", s_userSettingsFileName );
						lua_pop( &io_luaState, 2 );
						return eae6320::Results::Failure;
					}
				}
				// Call the Lua function
				{
					constexpr int noArguments = 0;
					constexpr int noReturnValues = 0;
					constexpr int noErrorMessageHandler = 0;
					const auto luaResult = lua_pcall( &io_luaState, noArguments, noReturnValues, noErrorMessageHandler );
					if ( luaResult == LUA_OK )
					{
						return eae6320::Results::Success;
					}
					else
					{
						const std::string luaErrorMessage = lua_tostring( &io_luaState, -1 );
						lua_pop( &io_luaState, 1 );

						EAE6320_ASSERTF( false, "User settings file error: %s", luaErrorMessage.c_str() );
						if ( luaResult == LUA_ERRRUN )
						{
							eae6320::Logging::OutputError( "Error in the user settings file \"%s\": %s",
								s_userSettingsFileName, luaErrorMessage );
						}
						else
						{
							eae6320::Logging::OutputError( "Error processing the user settings file \"%s\": %s",
								s_userSettingsFileName, luaErrorMessage );
						}

						return eae6320::Results::InvalidFile;
					}
				}
			}
			else
			{
				const std::string luaErrorMessage = lua_tostring( &io_luaState, -1 );
				lua_pop( &io_luaState, 1 );

				if ( luaResult == LUA_ERRFILE )
				{
					EAE6320_ASSERTF( "Error opening or reading user settings file: %s", luaErrorMessage.c_str() );
					eae6320::Logging::OutputError( "Error opening or reading the user settings file \"%s\" even though it exists: %s",
						s_userSettingsFileName, luaErrorMessage.c_str() );

				}
				else if ( luaResult == LUA_ERRSYNTAX )
				{
					EAE6320_ASSERTF( "Syntax error in user settings file: %s", luaErrorMessage.c_str() );
					eae6320::Logging::OutputError( "Syntax error in the user settings file \"%s\": %s",
						s_userSettingsFileName, luaErrorMessage.c_str() );
				}
				else
				{
					EAE6320_ASSERTF( "Error loading user settings file: %s", luaErrorMessage.c_str() );
					eae6320::Logging::OutputError( "Error loading the user settings file \"%s\": %s",
						s_userSettingsFileName, luaErrorMessage.c_str() );
				}

				return eae6320::Results::InvalidFile;
			}
		}
		else
		{
			// If loading the file failed because the file doesn't exist it's ok;
			// default values will be used
			eae6320::Logging::OutputMessage( "The user settings file \"%s\" doesn't exist. Using default settings instead.",
				s_userSettingsFileName );
			return eae6320::Results::FileDoesntExist;
		}
	}

	eae6320::cResult PopulateUserSettingsFromLuaTable( lua_State& io_luaState )
	{
		auto result = eae6320::Results::Success;

		// Resolution Width
		{
			const char* key_width = "resolutionWidth";

			lua_pushstring( &io_luaState, key_width );
			lua_gettable( &io_luaState, -2 );
			if ( lua_isinteger( &io_luaState, -1 ) )
			{
				const auto luaInteger = lua_tointeger( &io_luaState, -1 );
				if ( luaInteger >= 0 )
				{
					constexpr auto maxWidth = ( 1u << ( sizeof( s_resolutionWidth ) * 8 ) ) - 1;
					if ( luaInteger <= maxWidth )
					{
						s_resolutionWidth = static_cast<uint16_t>( luaInteger );
						s_resolutionWidth_validity = eae6320::Results::Success;
						eae6320::Logging::OutputMessage( "User settings defined resolution width of %u", s_resolutionWidth );
					}
					else
					{
						s_resolutionWidth_validity = eae6320::Results::InvalidFile;
						eae6320::Logging::OutputMessage( "The user settings file %s specifies a resolution width (%i)"
							" that is bigger than the maximum (%u)", s_userSettingsFileName, luaInteger, maxWidth );
					}
				}
				else
				{
					s_resolutionWidth_validity = eae6320::Results::InvalidFile;
					eae6320::Logging::OutputMessage( "The user settings file %s specifies a non-positive resolution width (%i)",
						s_userSettingsFileName, luaInteger );
				}
			}
			else
			{
				s_resolutionWidth_validity = eae6320::Results::InvalidFile;
				eae6320::Logging::OutputMessage( "The user settings file %s specifies a %s for %s instead of an integer",
					s_userSettingsFileName, luaL_typename( &io_luaState, -1 ), key_width );
			}
			lua_pop( &io_luaState, 1 );
		}
		// Resolution Height
		{
			const char* key_height = "resolutionHeight";

			lua_pushstring( &io_luaState, key_height );
			lua_gettable( &io_luaState, -2 );
			if ( lua_isinteger( &io_luaState, -1 ) )
			{
				const auto luaInteger = lua_tointeger( &io_luaState, -1 );
				if ( luaInteger >= 0 )
				{
					constexpr auto maxHeight = ( 1u << ( sizeof( s_resolutionHeight ) * 8 ) ) - 1;
					if ( luaInteger <= maxHeight )
					{
						s_resolutionHeight = static_cast<uint16_t>( luaInteger );
						s_resolutionHeight_validity = eae6320::Results::Success;
						eae6320::Logging::OutputMessage( "User settings defined resolution height of %u", s_resolutionHeight );
					}
					else
					{
						s_resolutionHeight_validity = eae6320::Results::InvalidFile;
						eae6320::Logging::OutputMessage( "The user settings file %s specifies a resolution height (%i)"
							" that is bigger than the maximum (%u)", s_userSettingsFileName, luaInteger, maxHeight );
					}
				}
				else
				{
					s_resolutionHeight_validity = eae6320::Results::InvalidFile;
					eae6320::Logging::OutputMessage( "The user settings file %s specifies a non-positive resolution height (%i)",
						s_userSettingsFileName, luaInteger );
				}
			}
			else
			{
				s_resolutionWidth_validity = eae6320::Results::InvalidFile;
				eae6320::Logging::OutputMessage( "The user settings file %s specifies a %s for %s instead of an integer",
					s_userSettingsFileName, luaL_typename( &io_luaState, -1 ), key_height );
			}
			lua_pop( &io_luaState, 1 );
		}

		return result;
	}
}
