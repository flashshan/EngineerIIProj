/*
This file contains macros that can be used in a class declaration
to make that class a reference-counted asset

If you're interested, see the cbReferenceCountedAsset files
for a different way of doing this that is more idiomatic C++.
The drawback of using a base class is that
the reference count will always come first in the derived class,
and that can make the derived class substantially bigger than it needs to be.
This version using macros allows the reference count to be located
anywhere in the class.
*/

#ifndef EAE6320_ASSETS_REFERENCECOUNTEDASSETS_H
#define EAE6320_ASSETS_REFERENCECOUNTEDASSETS_H

// Include Files
//==============

#include <cstdint>

#include <Engine/Asserts/Asserts.h>
#include <limits>

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#endif

// Reference Counting
//-------------------

#if defined( EAE6320_PLATFORM_WINDOWS )

// The "interlocked" family of functions are atomic:
//	* If two threads call InterlockedIncrementNoFence16() at the same time there is no guarantee which thread will increment it first,
//		but it is guaranteed that it will be incremented exactly two times
//	* If two threads call InterlockedDecrementNoFence16() at the same time there is no guarantee which thread will decrement it first,
//		but it is guaranteed that it will be decremented exactly two times
//		and that the returned decremented reference count will be unique in each of those threads
//		(and, more importantly, will only be zero in exactly one of those threads
//		which means that the reference-counted asset will only be deleted once)
//	* The reference count must be 16-bit aligned

#define EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()	\
		void IncrementReferenceCount()	\
		{	\
			EAE6320_ASSERT( ( m_referenceCount > 0 ) && ( m_referenceCount < std::numeric_limits<decltype( m_referenceCount )>::max() ) );	\
			EAE6320_ASSERT( ( reinterpret_cast<uintptr_t>( &m_referenceCount ) % 2 ) == 0 );	\
			auto* const referenceCount_asSigned = reinterpret_cast<short*>( &m_referenceCount );	\
			InterlockedIncrementNoFence16( referenceCount_asSigned );	\
		}	\
		uint16_t DecrementReferenceCount()	\
		{	\
			EAE6320_ASSERT( m_referenceCount > 0 );	\
			EAE6320_ASSERT( ( reinterpret_cast<uintptr_t>( &m_referenceCount ) % 2 ) == 0 );	\
			auto* const referenceCount_asSigned = reinterpret_cast<short*>( &m_referenceCount );	\
			const auto newReferenceCount = InterlockedDecrementNoFence16( referenceCount_asSigned );	\
			if ( newReferenceCount == 0 ) delete this;	\
			return newReferenceCount;	\
		}

#else
#error "No implementation exists for reference counting on this platform"
#endif

// Initialization / Clean Up
//--------------------------

// Reference-counted assets can't be copied or assigned to
#define EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS( tClassName )	\
	tClassName( const tClassName& i_instanceToBeCopied ) = delete;	\
	tClassName& operator =( const tClassName& i_instanceToBeCopied ) = delete;	\
	tClassName( tClassName&& io_instanceToBeMoved ) = delete;	\
	tClassName& operator =( tClassName&& io_instanceToBeMoved ) = delete;

// Data
//=====

#define EAE6320_ASSETS_DECLAREREFERENCECOUNT() uint16_t m_referenceCount = 1;

#endif	// EAE6320_ASSETS_REFERENCECOUNTEDASSETS_H
