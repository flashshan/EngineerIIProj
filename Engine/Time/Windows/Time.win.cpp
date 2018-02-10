// Include Files
//==============

#include "../Time.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Windows/Includes.h>
#include <Engine/Windows/Functions.h>

// Static Data Initialization
//===========================

namespace
{
	double s_secondsPerTick = 0.0;
}

// Interface
//==========

// Time
//-----

uint64_t eae6320::Time::GetCurrentSystemTimeTickCount()
{
	LARGE_INTEGER totalTickCountSinceSystemBoot;
	const auto result = QueryPerformanceCounter( &totalTickCountSinceSystemBoot );
	// Microsoft claims that querying the counter will never fail on Windows XP or later:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
	EAE6320_ASSERTF( result != FALSE, "QueryPerformanceCounter() failed" );
	return static_cast<uint64_t>( totalTickCountSinceSystemBoot.QuadPart );
}

double eae6320::Time::ConvertTicksToSeconds( const uint64_t i_tickCount )
{
	EAE6320_ASSERT( s_secondsPerTick > 0.0 );
	return static_cast<double>( i_tickCount ) * s_secondsPerTick;
}

uint64_t eae6320::Time::ConvertSecondsToTicks( const double i_secondCount )
{
	EAE6320_ASSERT( s_secondsPerTick > 0.0 );
	return static_cast<uint64_t>( ( i_secondCount / s_secondsPerTick ) + 0.5 );
}

double eae6320::Time::ConvertRatePerSecondToRatePerTick( const double i_rate_perSecond )
{
	EAE6320_ASSERT( s_secondsPerTick > 0.0 );
	return i_rate_perSecond * s_secondsPerTick;
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Time::Initialize()
{
	auto result = Results::Success;

	// Get the frequency of the high-resolution performance counter
	{
		// Microsoft claims that querying the frequency will never fail on Windows XP or later:
		// https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
		LARGE_INTEGER ticksPerSecond;
		if ( QueryPerformanceFrequency( &ticksPerSecond ) != FALSE )
		{
			if ( ticksPerSecond.QuadPart != 0 )
			{
				s_secondsPerTick = 1.0 / static_cast<double>( ticksPerSecond.QuadPart );
			}
			else
			{
				result = Results::Failure;
				EAE6320_ASSERT( false );
				Logging::OutputMessage( "This hardware doesn't support high resolution performance counters!" );
				goto OnExit;
			}
		}
		else
		{
			result = Results::Failure;
			const auto errorMessage = Windows::GetLastSystemError();
			EAE6320_ASSERTF( false, errorMessage.c_str() );
			Logging::OutputMessage( "Windows failed to query performance frequency: %s", errorMessage.c_str() );
			goto OnExit;
		}
	}

	Logging::OutputMessage( "Initialized time" );

OnExit:

	return result;
}

eae6320::cResult eae6320::Time::CleanUp()
{
	return Results::Success;
}
