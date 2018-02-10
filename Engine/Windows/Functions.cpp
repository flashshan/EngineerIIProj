// Include Files
//==============

#include "Functions.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Platform/Platform.h>
#include <iostream>
#include <regex>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <sstream>

// Helper Function Declarations
//=============================

namespace
{
	void OutputMessageForVisualStudio( const char* const i_severity, const char* const i_errorMessage, const char* const i_optionalFilePath,
		const unsigned int* const i_optionalLineNumber, const unsigned int* const i_optionalColumnNumber );
}

// Interface
//==========

eae6320::cResult eae6320::Windows::CopyFile( const char* const i_path_source, const char* const i_path_target,
	const bool i_shouldFunctionFailIfTargetAlreadyExists, const bool i_shouldTargetFileTimeBeModified,
	std::string* const o_errorMessage )
{
	auto result = Results::Success;

	HANDLE fileHandle = INVALID_HANDLE_VALUE;

	// Copy the file
	if ( ::CopyFile( i_path_source, i_path_target, i_shouldFunctionFailIfTargetAlreadyExists ) != FALSE )
	{
		// Update the new file's timestamp if requested
		if ( i_shouldTargetFileTimeBeModified )
		{
			// Get the current system time
			FILETIME fileTime;
			{
				SYSTEMTIME systemTime;
				GetSystemTime( &systemTime );
				if ( SystemTimeToFileTime( &systemTime, &fileTime ) == FALSE )
				{
					if ( o_errorMessage )
					{
						*o_errorMessage = GetLastSystemError();
					}
					result = Results::Failure;
					goto OnExit;
				}
			}
			// Set the current system time as the file's last write time
			{
				{
					const DWORD desiredAccess = FILE_WRITE_ATTRIBUTES;
					const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
					SECURITY_ATTRIBUTES* useDefaultSecurity = nullptr;
					const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
					const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
					const HANDLE dontUseTemplateFile = NULL;
					fileHandle = CreateFile( i_path_target, desiredAccess, otherProgramsCanStillReadTheFile,
						useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile );
					if ( fileHandle == INVALID_HANDLE_VALUE )
					{
						if ( o_errorMessage )
						{
							*o_errorMessage = GetLastSystemError();
						}
						result = Results::Failure;
						goto OnExit;
					}
				}
				{
					FILETIME* const onlyChangeLastWriteTime = nullptr;
					if ( SetFileTime( fileHandle, onlyChangeLastWriteTime, onlyChangeLastWriteTime, &fileTime ) == FALSE )
					{
						if ( o_errorMessage )
						{
							*o_errorMessage = GetLastSystemError();
						}
						result = Results::Failure;
						goto OnExit;
					}
				}
			}
		}
	}
	else
	{
		DWORD windowsErrorCode;
		const auto windowsErrorMessage = GetLastSystemError( &windowsErrorCode );
		switch ( windowsErrorCode )
		{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			result = Results::FileDoesntExist;
			break;
		default:
			result = Results::Failure;
		}
		if ( o_errorMessage )
		{
			*o_errorMessage = windowsErrorMessage;
		}
		goto OnExit;
	}

OnExit:

	if ( fileHandle != INVALID_HANDLE_VALUE )
	{
		if ( CloseHandle( fileHandle ) == FALSE )
		{
			const auto windowsErrorMessage = GetLastSystemError();
			if ( o_errorMessage )
			{
				*o_errorMessage += "\n";
				*o_errorMessage += windowsErrorMessage;
			}
			if ( result )
			{
				result = Results::Failure;
			}
		}
		fileHandle = INVALID_HANDLE_VALUE;
	}

	return result;
}

eae6320::cResult eae6320::Windows::CreateDirectoryIfItDoesntExist( const std::string& i_filePath, std::string* const o_errorMessage )
{
	auto result = Results::Success;

	// Manipulate the path into a form that SHCreateDirectoryEx() likes
	std::string directory;
	{
		directory = i_filePath;
		// Replace all slashes with back slashes
		for ( auto pos_slash = directory.find( '/' ); pos_slash != directory.npos; pos_slash = directory.find( '/', pos_slash + 1 ) )
		{
			directory.at( pos_slash ) = '\\';
		}
		// If the path is to a file (likely), remove it so that only the directory remains
		{
			const auto pos_slash = directory.find_last_of( '\\' );
			if ( pos_slash != directory.npos )
			{
				directory = directory.substr( 0, pos_slash );
			}
		}
	}
	// Resolve the path so it has no ".."s.
	constexpr DWORD maxCharacterCount = MAX_PATH;
	char buffer[maxCharacterCount];
	{
		if ( PathCanonicalize( buffer, directory.c_str() ) != FALSE )
		{
			// Create the directory
			int windowsErrorCode;
			{
				constexpr HWND noWindowIsDisplayed = NULL;
				constexpr SECURITY_ATTRIBUTES* const useDefaultSecurityAttributes = nullptr;
				windowsErrorCode = SHCreateDirectoryEx( noWindowIsDisplayed, buffer, useDefaultSecurityAttributes );
			}
			if ( ( windowsErrorCode != ERROR_SUCCESS ) && ( ( windowsErrorCode != ERROR_FILE_EXISTS ) && ( windowsErrorCode != ERROR_ALREADY_EXISTS ) ) )
			{
				if ( o_errorMessage )
				{
					*o_errorMessage = GetFormattedSystemMessage( result );
				}
				result = Results::Failure;
				goto OnExit;
			}
		}
		else
		{
			if ( o_errorMessage )
			{
				*o_errorMessage = GetLastSystemError();
			}
			result = Results::Failure;
			goto OnExit;
		}
	}

OnExit:

	return result;
}

bool eae6320::Windows::DoesFileExist( const char* const i_path, std::string* const o_errorMessage )
{
	// Try to get information about the file
	WIN32_FIND_DATA fileData;
	HANDLE file = FindFirstFile( i_path, &fileData );
	if ( file != INVALID_HANDLE_VALUE )
	{
		if ( FindClose( file ) == FALSE )
		{
			const std::string windowsErrorMessage = GetLastSystemError();
			EAE6320_ASSERTF( false, "Windows failed to close the file handle to \"%s\" after finding it: %s",
				i_path, windowsErrorMessage.c_str() );
			if ( o_errorMessage )
			{
				std::ostringstream errorMessage;
				errorMessage << "Windows failed to close the file handle to \"" << i_path << "\" after finding it: " << windowsErrorMessage;
				*o_errorMessage = errorMessage.str();
			}
		}
		return true;
	}
	else
	{
		DWORD errorCode;
		std::string errorMessage = GetLastSystemError( &errorCode );
		EAE6320_ASSERTF( ( ( errorCode == ERROR_FILE_NOT_FOUND ) || ( errorCode == ERROR_PATH_NOT_FOUND ) ),
			"FindFirstFile() failed with the unexpected error code of %u: %s", errorCode, errorMessage.c_str() );
		if ( o_errorMessage )
		{
			*o_errorMessage = errorMessage;
		}
		return false;
	}
}

eae6320::cResult eae6320::Windows::ExecuteCommand( const char* const i_command, DWORD* const o_exitCode, std::string* const o_errorMessage )
{
	return ExecuteCommand( NULL, i_command, o_exitCode, o_errorMessage );
}

eae6320::cResult eae6320::Windows::ExecuteCommand( const char* const i_path, const char* const i_optionalArguments,
	DWORD* const o_exitCode, std::string* const o_errorMessage )
{
	// Get a non-const char* command line
	std::string path;
	constexpr DWORD argumentBufferSize = 1024;
	char arguments[argumentBufferSize];
	{
		std::string optionalArguments( i_optionalArguments );
		if ( i_path )
		{
			path = i_path;
		}
		else
		{
			// If the path is part of the optional arguments then separate it
			const auto pos_firstNonSpace = optionalArguments.find_first_not_of( " \t" );
			const auto pos_firstNonQuote = optionalArguments.find_first_not_of( '\"', pos_firstNonSpace );
			const auto quoteCountBeginning = pos_firstNonQuote - pos_firstNonSpace;
			if ( quoteCountBeginning == 0 )
			{
				// If there are no quotes then the command ends at the first space
				const auto pos_firstSpaceAfterCommand = optionalArguments.find_first_of( " \t", pos_firstNonQuote );
				path = optionalArguments.substr( pos_firstNonQuote, pos_firstSpaceAfterCommand - pos_firstNonQuote );
				const auto pos_firstNonSpaceAfterCommand = optionalArguments.find_first_not_of( " \t", pos_firstSpaceAfterCommand );
				optionalArguments = optionalArguments.substr( pos_firstNonSpaceAfterCommand );
			}
			else
			{
				// If there are quotes then the command ends at the next quote
				const auto pos_firstQuoteAfterCommand = optionalArguments.find_first_of( '\"', pos_firstNonQuote );
				path = optionalArguments.substr( pos_firstNonQuote, pos_firstQuoteAfterCommand - pos_firstNonQuote );
				const auto pos_firstNonQuoteAfterCommand = optionalArguments.find_first_not_of( '\"', pos_firstQuoteAfterCommand );
				const auto quoteCountAfterCommand = pos_firstNonQuoteAfterCommand - pos_firstQuoteAfterCommand;
				const auto pos_firstNonSpaceAfterCommand = optionalArguments.find_first_not_of( " \t", pos_firstNonQuoteAfterCommand );
				optionalArguments = optionalArguments.substr( pos_firstNonSpaceAfterCommand );
				// If the entire command line was surrounded by quotes the trailing ones must be removed
				if ( quoteCountAfterCommand < quoteCountBeginning )
				{
					for ( auto quoteCountToRemoveFromEnd = quoteCountBeginning - quoteCountAfterCommand;
						quoteCountToRemoveFromEnd > 0; --quoteCountToRemoveFromEnd )
					{
						const auto pos_lastQuote = optionalArguments.find_last_of( '\"' );
						if ( pos_lastQuote == ( optionalArguments.length() - 1 ) )
						{
							optionalArguments = optionalArguments.substr( 0, optionalArguments.length() - 1 );
						}
						else
						{
							EAE6320_ASSERTF( false, "Expected a trailing quote but didn't find it" );
							break;
						}
					}
				}
			}
		}
		// Create a single command line
		// (Windows doesn't require this, but if the application name is sent to CreateProcess()
		// then it won't show up as the first argument in main(),
		// which causes incorrect behavior to any program expecting standard command arguments)
		std::string commandLine;
		if ( !optionalArguments.empty() )
		{
			std::ostringstream argumentStream;
			argumentStream << path << " " << optionalArguments;
			commandLine = argumentStream.str();
		}
		else
		{
			commandLine = path;
		}
		// Copy it into a non-const buffer
		{
			const auto argumentLength = commandLine.length() + 1;
			if ( argumentBufferSize >= argumentLength )
			{
				strcpy_s( arguments, argumentBufferSize, commandLine.c_str() );
			}
			else
			{
				EAE6320_ASSERT( false );
				if ( o_errorMessage )
				{
					std::ostringstream errorMessage;
					errorMessage << "The non-const buffer of size " << argumentBufferSize
						<< " isn't big enough to hold the command line of length " << argumentLength;
					*o_errorMessage = errorMessage.str();
				}
				return Results::Failure;
			}
		}
	}
	
	// Start a new process
	auto result = Results::Success;
	constexpr SECURITY_ATTRIBUTES* useDefaultAttributes = nullptr;
	constexpr BOOL dontInheritHandles = FALSE;
	constexpr DWORD createDefaultProcess = 0;
	constexpr void* const useCallingProcessEnvironment = nullptr;
	constexpr char* const useCallingProcessCurrentDirectory = nullptr;
	STARTUPINFO startupInfo{};
	{
		startupInfo.cb = sizeof( startupInfo );
	}
	PROCESS_INFORMATION processInformation{};
	if ( CreateProcess( NULL, arguments, useDefaultAttributes, useDefaultAttributes,
		dontInheritHandles, createDefaultProcess, useCallingProcessEnvironment, useCallingProcessCurrentDirectory,
		&startupInfo, &processInformation ) != FALSE )
	{
		// Wait for the process to finish
		if ( WaitForSingleObject( processInformation.hProcess, INFINITE ) != WAIT_FAILED )
		{
			// Get the exit code
			if ( o_exitCode )
			{
				if ( GetExitCodeProcess( processInformation.hProcess, o_exitCode ) == FALSE )
				{
					const auto windowsErrorMessage = GetLastSystemError();
					result = Results::Failure;
					EAE6320_ASSERTF( false, "Couldn't get exit code of a process: %s", windowsErrorMessage.c_str() );
					if ( o_errorMessage )
					{
						std::ostringstream errorMessage;
						errorMessage << "Windows failed to get the exit code of the process \"" << path <<
							"\": " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
				}
			}
		}
		else
		{
			const auto windowsErrorMessage = GetLastSystemError();
			result = Results::Failure;
			EAE6320_ASSERTF( false, "Didn't wait for a process to finish: %s", windowsErrorMessage.c_str() );
			if ( o_errorMessage )
			{
				std::ostringstream errorMessage;
				errorMessage << "Windows failed to wait for the process \"" << path <<
					"\" to finish: " << windowsErrorMessage;
				*o_errorMessage = errorMessage.str();
			}
		}
		// Close the process handles
		if ( CloseHandle( processInformation.hProcess ) == FALSE )
		{
			const auto windowsErrorMessage = GetLastSystemError();
			if ( result )
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF( false, "Windows failed to close the handle to the process \"%s\""
				" after executing a command: %s", path.c_str(), windowsErrorMessage.c_str() );
			if ( o_errorMessage )
			{
				std::ostringstream errorMessage;
				errorMessage << "\nWindows failed to close the handle to the process \"" << path <<
					"\": " << windowsErrorMessage;
				*o_errorMessage += errorMessage.str();
			}
		}
		if ( CloseHandle( processInformation.hThread ) == FALSE )
		{
			const auto windowsErrorMessage = GetLastSystemError();
			if ( result )
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF( false, "Windows failed to close the handle to the process \"%s\" thread"
				" after executing a command: %s", path.c_str(), windowsErrorMessage.c_str() );
			if ( o_errorMessage )
			{
				std::ostringstream errorMessage;
				errorMessage << "\nWindows failed to close the handle to the process \"" << path <<
					"\" thread: " << windowsErrorMessage;
				*o_errorMessage += errorMessage.str();
			}
		}

		return result;
	}
	else
	{
		const auto windowsErrorMessage = GetLastSystemError();
		EAE6320_ASSERTF( false, "Couldn't start a process: %s", windowsErrorMessage.c_str() );
		if ( o_errorMessage )
		{
			std::ostringstream errorMessage;
			errorMessage << "Windows failed to start the process \"" << path << "\": " << windowsErrorMessage;
			*o_errorMessage = errorMessage.str();
		}
		return Results::Failure;
	}
}

eae6320::cResult eae6320::Windows::GetFilesInDirectory( const std::string& i_path, std::vector<std::string>& o_paths,
	const bool i_shouldSubdirectoriesBeSearchedRecursively, std::string* const o_errorMessage )
{
	auto result = Results::Success;

	HANDLE fileHandle = INVALID_HANDLE_VALUE;

	// Transform the path to have a trailing slash
	// (note that this code sacrifices robustness for simplicity;
	// there are several cases of unexpected input that would cause it to behave incorrectly)
	std::string path_trailingSlash;
	{
		path_trailingSlash = i_path;
		const auto pos_lastSlash = path_trailingSlash.find_last_of( "\\/" );
		if ( pos_lastSlash != path_trailingSlash.npos )
		{
			if ( pos_lastSlash != ( path_trailingSlash.length() - 1 ) )
			{
				path_trailingSlash += "\\";
			}
		}
		else
		{
			path_trailingSlash += "\\";
		}
	}
	// Add a star at the end
	// (this causes FindFirstFile()/FindNextFile() to find the files in the directory rather than just the directory itself)
	const auto path_trailingStar = path_trailingSlash + "*";
	// Find the first matching file
	WIN32_FIND_DATA fileData;
	fileHandle = FindFirstFile( path_trailingStar.c_str(), &fileData );
	if ( fileHandle != INVALID_HANDLE_VALUE )
	{
		// Process each matching file
		do
		{
			// The first files found are . and ..
			if ( fileData.cFileName[0] != '.' )
			{
				const auto path = path_trailingSlash + fileData.cFileName;
				if ( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if ( i_shouldSubdirectoriesBeSearchedRecursively )
					{
						if ( !( result = GetFilesInDirectory( path, o_paths, i_shouldSubdirectoriesBeSearchedRecursively, o_errorMessage ) ) )
						{
							goto OnExit;
						}
					}
				}
				else
				{
					o_paths.push_back( path );
				}
			}
		// Find the next matching file
		} while ( FindNextFile( fileHandle, &fileData ) != FALSE );
		
		// Verify that the loop exited because all matching files were found
		DWORD errorCode;
		const auto windowsErrorMessage = GetLastSystemError( &errorCode );
		if ( errorCode != ERROR_NO_MORE_FILES )
		{
			result = Results::Failure;
			if ( o_errorMessage )
			{
				*o_errorMessage = windowsErrorMessage;
			}
			goto OnExit;
		}
	}
	else
	{
		DWORD errorCode;
		const auto windowsErrorMessage = GetLastSystemError( &errorCode );
		if ( o_errorMessage )
		{
			*o_errorMessage = windowsErrorMessage;
		}
		switch ( errorCode )
		{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			result = Results::FileDoesntExist;
			break;
		default:
			result = Results::Failure;
		}
		goto OnExit;
	}

OnExit:

	if ( fileHandle != INVALID_HANDLE_VALUE )
	{
		if ( FindClose( fileHandle ) == FALSE )
		{
			if ( o_errorMessage )
			{
				const auto windowsErrorMessage = GetLastSystemError();
				*o_errorMessage += "\n";
				*o_errorMessage += windowsErrorMessage;
			}
			if ( result )
			{
				result = Results::Failure;
			}
		}
	}

	return result;
}

eae6320::cResult eae6320::Windows::GetEnvironmentVariable( const char* const i_key, std::string& o_value, std::string* const o_errorMessage )
{
	// Windows requires a character buffer
	// to copy the environment variable into.
	// An arbitrary value is chosen that "should" be "big enough":
	constexpr DWORD maxCharacterCount = MAX_PATH;
	char buffer[maxCharacterCount];
	// Ask Windows for the environment variable
	const auto characterCount = ::GetEnvironmentVariable( i_key, buffer, maxCharacterCount );
	if ( characterCount > 0 )
	{
		if ( characterCount <= maxCharacterCount )
		{
			o_value = buffer;
			return Results::Success;
		}
		else
		{
			// If you're seeing this error you will need to increase maxCharacterCount
			if ( o_errorMessage )
			{
				std::ostringstream errorMessage;
				errorMessage << "The environment variable \"" << i_key << "\" requires " << characterCount <<
					" characters and the provided buffer only has room for " << maxCharacterCount;
				*o_errorMessage = errorMessage.str();
			}
			return Results::Failure;
		}
	}
	else
	{
		auto result = Results::Failure;
		{
			DWORD errorCode;
			const auto errorString = GetLastSystemError( &errorCode );
			if ( errorCode == ERROR_ENVVAR_NOT_FOUND )
			{
				result = Results::Platform::EnvironmentVariableDoesntExist;
				if ( o_errorMessage )
				{
					// If you're seeing this error and the environment variable is spelled correctly
					// it _probably_ means that you are debugging and haven't set up the environment.
					//	* Right click the project name and choose "Properties"
					//	* In the tree view to the left select "Configuration Properties->Debugging"
					//	* In the "Environment" field make sure that you have this environment variable set with no whitespace like:
					//		* someKey=someValue
					std::ostringstream errorMessage;
					errorMessage << "The environment variable \"" << i_key << "\" doesn't exist";
					*o_errorMessage = errorMessage.str();
				}
			}
			else if ( o_errorMessage )
			{
				std::ostringstream errorMessage;
				errorMessage << "Windows failed to get the environment variable \"" << i_key << "\": " << errorString;
				*o_errorMessage = errorMessage.str();
			}
		}
		return Results::Failure;
	}
}

std::string eae6320::Windows::GetFormattedSystemMessage( const DWORD i_code )
{
	std::string errorMessage;
	{
		constexpr DWORD formattingOptions =
			// Get the error message from Windows
			FORMAT_MESSAGE_FROM_SYSTEM
			// The space for the error message should be allocated by Windows
			| FORMAT_MESSAGE_ALLOCATE_BUFFER
			// Any potentially deferred inserts should be ignored
			// (Unfortunately, there are some system messages that have deferred inserts,
			// but there is no way to determine whether a given code has inserts or not
			// and so the only safe way to make a general formatting function like this one
			// is to ignore any)
			| FORMAT_MESSAGE_IGNORE_INSERTS;
		constexpr void* const messageIsFromWindows = nullptr;
		constexpr DWORD useTheDefaultLanguage = 0;
		char* messageBuffer = nullptr;
		constexpr DWORD minimumCharacterCountToAllocate = 1;
		va_list* const insertsAreIgnored = nullptr;
		const auto storedCharacterCount = FormatMessage( formattingOptions, messageIsFromWindows, i_code,
			useTheDefaultLanguage, reinterpret_cast<LPSTR>( &messageBuffer ), minimumCharacterCountToAllocate, insertsAreIgnored );
		if ( storedCharacterCount != 0 )
		{
			errorMessage = messageBuffer;
			// Trailing carriage returns are unnecessary
			errorMessage = errorMessage.substr( 0, errorMessage.find_last_not_of( "\r\n" ) + 1 );
		}
		else
		{
			const auto newErrorCode = GetLastError();

			std::ostringstream formattedErrorMessage;
			formattedErrorMessage << "Windows System Code " << i_code;
			if ( newErrorCode == 317 )
			{
				formattedErrorMessage << " (no text description exists)";
			}
			else
			{
				formattedErrorMessage << " (Windows failed to format a text description with error code " << newErrorCode << ")";
			}
			errorMessage = formattedErrorMessage.str();
		}
		// Try to free the memory regardless of whether formatting worked or not,
		// and ignore any error messages
		LocalFree( messageBuffer );
	}
	return errorMessage;
}

std::string eae6320::Windows::GetLastSystemError( DWORD* const o_optionalErrorCode )
{
	// Windows stores the error as a code
	const auto errorCode = GetLastError();
	if ( o_optionalErrorCode )
	{
		*o_optionalErrorCode = errorCode;
	}
	return GetFormattedSystemMessage( errorCode );
}

eae6320::cResult eae6320::Windows::GetLastWriteTime( const char* const i_path, uint64_t& o_lastWriteTime, std::string* const o_errorMessage )
{
	// Get the last time that the file was written to
	ULARGE_INTEGER lastWriteTime;
	{
		WIN32_FIND_DATA fileData;
		{
			const auto file = FindFirstFile( i_path, &fileData );
			if ( file != INVALID_HANDLE_VALUE )
			{
				if ( FindClose( file ) == FALSE )
				{
					if ( o_errorMessage )
					{
						*o_errorMessage = GetLastSystemError();
					}
					return Results::Failure;
				}
			}
			else
			{
				DWORD errorCode;
				const auto windowsErrorMessage = GetLastSystemError( &errorCode );
				if ( o_errorMessage )
				{
					*o_errorMessage = windowsErrorMessage;
				}
				switch ( errorCode )
				{
				case ERROR_FILE_NOT_FOUND:
				case ERROR_PATH_NOT_FOUND:
					return Results::FileDoesntExist;
					break;
				default:
					return Results::Failure;
				}
			}
		}
		FILETIME fileTime = fileData.ftLastWriteTime;
		lastWriteTime.HighPart = fileTime.dwHighDateTime;
		lastWriteTime.LowPart = fileTime.dwLowDateTime;
	}
	o_lastWriteTime = static_cast<uint64_t>( lastWriteTime.QuadPart );
	return Results::Success;
}

eae6320::cResult eae6320::Windows::InvalidateLastWriteTime( const char* const i_path, std::string* const o_errorMessage )
{
	auto result = Results::Success;

	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	{
		constexpr DWORD desiredAccess = FILE_WRITE_ATTRIBUTES;
		constexpr DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
		constexpr SECURITY_ATTRIBUTES* const useDefaultSecurity = nullptr;
		constexpr DWORD onlySucceedIfFileExists = OPEN_EXISTING;
		constexpr DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
		constexpr HANDLE dontUseTemplateFile = NULL;
		fileHandle = CreateFile( i_path, desiredAccess, otherProgramsCanStillReadTheFile,
			useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile );
		if ( fileHandle == INVALID_HANDLE_VALUE )
		{
			DWORD errorCode;
			const auto windowsErrorMessage = GetLastSystemError( &errorCode );
			if ( o_errorMessage )
			{
				*o_errorMessage = windowsErrorMessage;
			}
			switch ( errorCode )
			{
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				result = Results::FileDoesntExist;
				break;
			default:
				result = Results::Failure;
			}
			goto OnExit;
		}
	}
	{
		FILETIME earliestPossibleTime{};
		{
			SYSTEMTIME systemTime{};
			{
				systemTime.wYear = 1980;
				systemTime.wMonth = 1;
				systemTime.wDay = 1;
			}
			if ( SystemTimeToFileTime( &systemTime, &earliestPossibleTime ) == FALSE )
			{
				if ( o_errorMessage )
				{
					*o_errorMessage = GetLastSystemError();
				}
				result = Results::Failure;
				goto OnExit;
			}
		}
		FILETIME* const onlyChangeLastWriteTime = nullptr;
		if ( SetFileTime( fileHandle, onlyChangeLastWriteTime, onlyChangeLastWriteTime, &earliestPossibleTime ) == FALSE )
		{
			if ( o_errorMessage )
			{
				*o_errorMessage = GetLastSystemError();
			}
			result = Results::Failure;
			goto OnExit;
		}
	}

OnExit:

	if ( fileHandle != INVALID_HANDLE_VALUE )
	{
		if ( CloseHandle( fileHandle ) == FALSE )
		{
			if ( o_errorMessage )
			{
				const auto windowsErrorMessage = GetLastSystemError();
				*o_errorMessage += "\n";
				*o_errorMessage += windowsErrorMessage;
			}
			result = Results::Failure;
		}
		fileHandle = INVALID_HANDLE_VALUE;
	}

	return result;
}

eae6320::cResult eae6320::Windows::LoadBinaryFile( const char* const i_path, sDataFromFile& o_data, std::string* const o_errorMessage )
{
	auto result = Results::Success;

	// Initialize the output struct so that if there's an error during this function any existing garbage data isn't misinterpreted
	{
		o_data.data = nullptr;
		o_data.size = 0;
	}

	// Open the file
	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	{
		constexpr DWORD desiredAccess = FILE_GENERIC_READ;
		constexpr DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
		constexpr SECURITY_ATTRIBUTES* const useDefaultSecurity = nullptr;
		constexpr DWORD onlySucceedIfFileExists = OPEN_EXISTING;
		constexpr DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
		constexpr HANDLE dontUseTemplateFile = NULL;
		fileHandle = CreateFile( i_path, desiredAccess, otherProgramsCanStillReadTheFile,
			useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile );
		if ( fileHandle == INVALID_HANDLE_VALUE )
		{
			DWORD errorCode;
			const auto windowsError = eae6320::Windows::GetLastSystemError( &errorCode );
			switch ( errorCode )
			{
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				result = Results::FileDoesntExist;
				break;
			default:
				result = Results::Failure;
			}
			if ( o_errorMessage )
			{
				std::ostringstream errorMessage;
				errorMessage << "Windows failed to open the file \"" << i_path << "\" for reading: " << windowsError;
				*o_errorMessage = errorMessage.str();
			}
			goto OnExit;
		}
	}
	// Get the file's size
	{
		LARGE_INTEGER fileSize_integer;
		if ( GetFileSizeEx( fileHandle, &fileSize_integer ) != FALSE )
		{
			EAE6320_ASSERT( fileSize_integer.QuadPart <= SIZE_MAX );
			o_data.size = static_cast<size_t>( fileSize_integer.QuadPart );
		}
		else
		{
			if ( o_errorMessage )
			{
				const auto windowsError = eae6320::Windows::GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "Windows failed to get the size of the file \"" << i_path << "\": " << windowsError;
				*o_errorMessage = errorMessage.str();
			}
			result = Results::Failure;
			goto OnExit;
		}
	}
	// Read the file's contents into allocated memory
	o_data.data = malloc( o_data.size );
	if ( o_data.data )
	{
		DWORD bytesReadCount;
		constexpr OVERLAPPED* const readSynchronously = nullptr;
		EAE6320_ASSERT( o_data.size < ( uint64_t( 1u ) << ( sizeof( bytesReadCount ) * 8 ) ) );
		if ( ReadFile( fileHandle, o_data.data, static_cast<DWORD>( o_data.size ),
			&bytesReadCount, readSynchronously ) == FALSE )
		{
			if ( o_errorMessage )
			{
				const auto windowsError = eae6320::Windows::GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "Windows failed to read the contents of the file \"" << i_path << "\": " << windowsError;
				*o_errorMessage = errorMessage.str();
			}
			result = Results::Failure;
			goto OnExit;
		}
	}
	else
	{
		if ( o_errorMessage )
		{
			std::ostringstream errorMessage;
			errorMessage << "Failed to allocate " << o_data.size << " bytes to read in the file \"" << i_path << "\"";
			*o_errorMessage = errorMessage.str();
		}
		result = Results::OutOfMemory;
		goto OnExit;
	}

OnExit:

	if ( !result )
	{
		if ( o_data.data )
		{
			o_data.Free();
		}
	}
	if ( fileHandle != INVALID_HANDLE_VALUE )
	{
		if ( CloseHandle( fileHandle ) == FALSE )
		{
			if ( o_errorMessage )
			{
				const auto windowsError = eae6320::Windows::GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "\nWindows failed to close the file handle from \"" << i_path << "\": " << windowsError;
				*o_errorMessage += errorMessage.str();
			}
			if ( result )
			{
				result = Results::Failure;
			}
		}
		fileHandle = INVALID_HANDLE_VALUE;
	}

	return result;
}

void eae6320::Windows::OutputErrorMessageForVisualStudio( const char* const i_errorMessage, const char* const i_optionalFilePath,
	const unsigned int* const i_optionalLineNumber, const unsigned int* const i_optionalColumnNumber )
{
	OutputMessageForVisualStudio( "error", i_errorMessage, i_optionalFilePath, i_optionalLineNumber, i_optionalColumnNumber );
}

void eae6320::Windows::OutputWarningMessageForVisualStudio( const char* const i_errorMessage, const char* const i_optionalFilePath,
	const unsigned int* const i_optionalLineNumber, const unsigned int* const i_optionalColumnNumber )
{
	OutputMessageForVisualStudio( "warning", i_errorMessage, i_optionalFilePath, i_optionalLineNumber, i_optionalColumnNumber );
}

eae6320::cResult eae6320::Windows::WriteBinaryFile( const char* const i_path, const void* const i_data, const size_t i_size, std::string* const o_errorMessage )
{
	auto result = Results::Success;

	// Open the file
	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	{
		constexpr DWORD desiredAccess = FILE_GENERIC_WRITE;
		constexpr DWORD noOtherProgramsCanShareAccess = 0;
		constexpr SECURITY_ATTRIBUTES* const useDefaultSecurity = nullptr;
		constexpr DWORD alwaysCreateANewFile = CREATE_ALWAYS;
		constexpr DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
		constexpr HANDLE dontUseTemplateFile = nullptr;
		fileHandle = CreateFile( i_path, desiredAccess, noOtherProgramsCanShareAccess,
			useDefaultSecurity, alwaysCreateANewFile, useDefaultAttributes, dontUseTemplateFile );
		if ( fileHandle == INVALID_HANDLE_VALUE )
		{
			if ( o_errorMessage )
			{
				const std::string windowsError = eae6320::Windows::GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "Windows failed to open the file \"" << i_path << "\" for writing: " << windowsError;
				*o_errorMessage = errorMessage.str();
			}
			result = Results::Failure;
			goto OnExit;
		}
	}
	// Write the modified shader source
	{
		constexpr OVERLAPPED* const writeSynchronously = nullptr;
		EAE6320_ASSERT( i_size < ( uint64_t( 1u ) << ( sizeof( DWORD ) * 8 ) ) );
		const auto bytesToWriteCount = static_cast<DWORD>( i_size );
		DWORD bytesWrittenCount;
		if ( WriteFile( fileHandle, i_data, bytesToWriteCount, &bytesWrittenCount, writeSynchronously ) != FALSE )
		{
			if ( bytesWrittenCount != bytesToWriteCount )
			{
				if ( o_errorMessage )
				{
					std::ostringstream errorMessage;
					errorMessage << "Windows was supposed to write " << bytesToWriteCount << " bytes to the file \"" << i_path << "\", "
						"but only actually wrote " << bytesWrittenCount;
					*o_errorMessage = errorMessage.str();
				}
				result = Results::Failure;
				goto OnExit;
			}
		}
		else
		{
			if ( o_errorMessage )
			{
				const auto windowsError = eae6320::Windows::GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "Windows failed to write the file \"" << i_path << "\": " << windowsError;
				*o_errorMessage = errorMessage.str();
			}
			result = Results::Failure;
			goto OnExit;
		}
	}

OnExit:

	if ( fileHandle != INVALID_HANDLE_VALUE )
	{
		if ( CloseHandle( fileHandle ) == FALSE )
		{
			if ( result )
			{
				if ( o_errorMessage )
				{
					const std::string windowsError = eae6320::Windows::GetLastSystemError();
					std::ostringstream errorMessage;
					errorMessage << "\nWindows failed to close the file handle from \"" << i_path << "\": " << windowsError;
					*o_errorMessage += errorMessage.str();
				}
				result = Results::Failure;
			}
		}
		fileHandle = INVALID_HANDLE_VALUE;
	}

	return result;
}

// Helper Function Definitions
//============================

namespace
{
	void OutputMessageForVisualStudio( const char* const i_severity, const char* const i_errorMessage, const char* const i_optionalFilePath,
		const unsigned int* const i_optionalLineNumber, const unsigned int* const i_optionalColumnNumber )
	{
		if ( i_optionalFilePath )
		{
			std::cerr << i_optionalFilePath;
			if ( i_optionalLineNumber )
			{
				std::cerr << "(" << *i_optionalLineNumber;
				if ( i_optionalColumnNumber )
				{
					std::cerr << ", " << *i_optionalColumnNumber;
				}
				std::cerr << ")";
			}
			std::cerr << " : ";
		}
		std::cerr << i_severity << " : " << i_errorMessage
			// Using std::endl flushes the buffer so that the error shows up immediately
			<< std::endl;
	}
}
