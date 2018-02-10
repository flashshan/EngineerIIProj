// Include Files
//==============

#include "Functions.h"

#include <cstdarg>
#include <cstdio>
#include <Engine/Asserts/Asserts.h>
#include <iostream>
#include <sstream>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Functions.h>
#endif

// Helper Function Declarations
//=============================

namespace
{
	// Error / Warning Output
	//-----------------------

	eae6320::cResult FormatErrorOrWarningMessage( const char* const i_message, std::string &o_message, va_list io_insertions );

	void OutputErrorMessage_platformSpecific( const char* const i_errorMessage, const char* const i_optionalFilePath,
		const unsigned int* const i_optionalLineNumber, const unsigned int* const i_optionalColumnNumber );
	void OutputWarningMessage_platformSpecific( const char* const i_warningMessage, const char* const i_optionalFilePath,
		const unsigned int* const i_optionalLineNumber, const unsigned int* const i_optionalColumnNumber );
}

// Interface
//==========

// Error / Warning Output
//-----------------------

void eae6320::Assets::OutputErrorMessage( const char* const i_errorMessage, ... )
{
	std::string formattedMessage;
	eae6320::cResult result;
	{
		va_list insertions;
		va_start( insertions, i_errorMessage );
		result = FormatErrorOrWarningMessage( i_errorMessage, formattedMessage, insertions );
		va_end( insertions );
	}
	if ( result )
	{
		const char* const noPath = nullptr;
		const unsigned int* const noFileInfo = nullptr;
		OutputErrorMessage_platformSpecific( formattedMessage.c_str(), noPath, noFileInfo, noFileInfo );
	}
}
void eae6320::Assets::OutputErrorMessageWithFileInfo( const char* const i_filePath,
	const char* const i_errorMessage, ... )
{
	std::string formattedMessage;
	eae6320::cResult result;
	{
		va_list insertions;
		va_start( insertions, i_errorMessage );
		result = FormatErrorOrWarningMessage( i_errorMessage, formattedMessage, insertions );
		va_end( insertions );
	}
	if ( result )
	{
		const unsigned int* const noFileInfo = nullptr;
		OutputErrorMessage_platformSpecific( formattedMessage.c_str(), i_filePath, noFileInfo, noFileInfo );
	}
}
void eae6320::Assets::OutputErrorMessageWithFileInfo( const char* const i_filePath,
	const unsigned int i_lineNumber,
	const char* const i_errorMessage, ... )
{
	std::string formattedMessage;
	eae6320::cResult result;
	{
		va_list insertions;
		va_start( insertions, i_errorMessage );
		result = FormatErrorOrWarningMessage( i_errorMessage, formattedMessage, insertions );
		va_end( insertions );
	}
	if ( result )
	{
		const unsigned int* const noFileInfo = nullptr;
		OutputErrorMessage_platformSpecific( formattedMessage.c_str(), i_filePath, &i_lineNumber, noFileInfo );
	}
}
void eae6320::Assets::OutputErrorMessageWithFileInfo( const char* const i_filePath,
	const unsigned int i_lineNumber, const unsigned int i_columnNumber,
	const char* const i_errorMessage, ... )
{
	std::string formattedMessage;
	eae6320::cResult result;
	{
		va_list insertions;
		va_start( insertions, i_errorMessage );
		result = FormatErrorOrWarningMessage( i_errorMessage, formattedMessage, insertions );
		va_end( insertions );
	}
	if ( result )
	{
		OutputErrorMessage_platformSpecific( formattedMessage.c_str(), i_filePath, &i_lineNumber, &i_columnNumber );
	}
}

void eae6320::Assets::OutputWarningMessage( const char* const i_warningMessage, ... )
{
	std::string formattedMessage;
	eae6320::cResult result;
	{
		va_list insertions;
		va_start( insertions, i_warningMessage );
		result = FormatErrorOrWarningMessage( i_warningMessage, formattedMessage, insertions );
		va_end( insertions );
	}
	if ( result )
	{
		const char* const noPath = nullptr;
		const unsigned int* const noFileInfo = nullptr;
		OutputWarningMessage_platformSpecific( formattedMessage.c_str(), noPath, noFileInfo, noFileInfo );
	}
}
void eae6320::Assets::OutputWarningMessageWithFileInfo( const char* const i_filePath,
	const char* const i_warningMessage, ... )
{
	std::string formattedMessage;
	eae6320::cResult result;
	{
		va_list insertions;
		va_start( insertions, i_warningMessage );
		result = FormatErrorOrWarningMessage( i_warningMessage, formattedMessage, insertions );
		va_end( insertions );
	}
	if ( result )
	{
		const unsigned int* const noFileInfo = nullptr;
		OutputWarningMessage_platformSpecific( formattedMessage.c_str(), i_filePath, noFileInfo, noFileInfo );
	}
}
void eae6320::Assets::OutputWarningMessageWithFileInfo( const char* const i_filePath,
	const unsigned int i_lineNumber,
	const char* const i_warningMessage, ... )
{
	std::string formattedMessage;
	eae6320::cResult result;
	{
		va_list insertions;
		va_start( insertions, i_warningMessage );
		result = FormatErrorOrWarningMessage( i_warningMessage, formattedMessage, insertions );
		va_end( insertions );
	}
	if ( result )
	{
		const unsigned int* const noFileInfo = nullptr;
		OutputWarningMessage_platformSpecific( formattedMessage.c_str(), i_filePath, &i_lineNumber, noFileInfo );
	}
}
void eae6320::Assets::OutputWarningMessageWithFileInfo( const char* const i_filePath,
	const unsigned int i_lineNumber, const unsigned int i_columnNumber,
	const char* const i_warningMessage, ... )
{
	std::string formattedMessage;
	eae6320::cResult result;
	{
		va_list insertions;
		va_start( insertions, i_warningMessage );
		result = FormatErrorOrWarningMessage( i_warningMessage, formattedMessage, insertions );
		va_end( insertions );
	}
	if ( result )
	{
		OutputWarningMessage_platformSpecific( formattedMessage.c_str(), i_filePath, &i_lineNumber, &i_columnNumber );
	}
}

// Helper Function Definitions
//============================

namespace
{
	// Error / Warning Output
	//-----------------------

	eae6320::cResult FormatErrorOrWarningMessage( const char* const i_message, std::string &o_message, va_list io_insertions )
	{
		constexpr size_t bufferSize = 1024;
		char buffer[bufferSize];
		const auto formattingResult = vsnprintf( buffer, bufferSize, i_message, io_insertions );
		if ( formattingResult >= 0 )
		{
			if ( formattingResult < bufferSize )
			{
				o_message = buffer;
				return eae6320::Results::Success;
			}
			else
			{
				EAE6320_ASSERTF( false, "The internal asset build buffer of size %u isn't big enough to hold the formatted message of length %i",
					bufferSize, formattingResult + 1 );
				std::ostringstream errorMessage;
				errorMessage << "The internal buffer of size " << bufferSize
					<< " isn't big enough to hold the formmated message of length " << ( formattingResult + 1 ) << "!"
					" The unformatted message is: " << i_message;
				const unsigned int lineNumber = __LINE__;
				constexpr unsigned int* const noColumnNumber = nullptr;
				OutputErrorMessage_platformSpecific( errorMessage.str().c_str(), __FILE__, &lineNumber, noColumnNumber );
				return eae6320::Results::Failure;
			}
		}
		else
		{
			EAE6320_ASSERTF( false, "An encoding error occurred while formatting the asset build message \"%s\"", i_message );
			std::ostringstream errorMessage;
			errorMessage << "An encoding error was encountered! The unformatted message was: " << i_message;
			const unsigned int lineNumber = __LINE__;
			constexpr unsigned int* const noColumnNumber = nullptr;
			OutputErrorMessage_platformSpecific( errorMessage.str().c_str(), __FILE__, &lineNumber, noColumnNumber );
			return eae6320::Results::Failure;
		}
	}

	void OutputErrorMessage_platformSpecific( const char* const i_errorMessage, const char* const i_optionalFilePath,
		const unsigned int* const i_optionalLineNumber, const unsigned int* const i_optionalColumnNumber )
	{
#if defined( EAE6320_PLATFORM_WINDOWS )
		eae6320::Windows::OutputErrorMessageForVisualStudio( i_errorMessage, i_optionalFilePath, i_optionalLineNumber, i_optionalColumnNumber );
#else
	#error "No implementation exists for outputting asset build error messages!"
#endif
	}

	void OutputWarningMessage_platformSpecific( const char* const i_warningMessage, const char* const i_optionalFilePath,
		const unsigned int* const i_optionalLineNumber, const unsigned int* const i_optionalColumnNumber )
	{
#if defined( EAE6320_PLATFORM_WINDOWS )
		eae6320::Windows::OutputWarningMessageForVisualStudio( i_warningMessage, i_optionalFilePath, i_optionalLineNumber, i_optionalColumnNumber );
#else
	#error "No implementation exists for outputting asset build warning messages!"
#endif
	}
}
