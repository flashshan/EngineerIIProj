/*
	This file contains math-related functions
*/

#ifndef EAE6320_MATH_FUNCTIONS_H
#define EAE6320_MATH_FUNCTIONS_H

#include <type_traits>
#include <stdint.h>


namespace eae6320
{
	namespace Math
	{
		// Interface
		//==========

		uint8_t ClampColor(uint8_t i_value, uint8_t i_minValue, uint8_t i_maxValue);

		int RandomInt(int i_maxNumber);
		int RandomRangeInt(int i_minNumber, int i_maxNumber);
		float RandomRangeFloat(float i_minFloat, float i_maxFloat);


		float ConvertDegreesToRadians( const float i_degrees );
		float ConvertHorizontalFieldOfViewToVerticalFieldOfView( const float i_horizontalFieldOfView_inRadians,
			// aspectRatio = width / height
			const float i_aspectRatio );

		// Rounds up an unsigned integer to the next highest multiple
		template<typename tUnsignedInteger, class EnforceUnsigned = typename std::enable_if<std::is_unsigned<tUnsignedInteger>::value>::type>
			tUnsignedInteger RoundUpToMultiple( const tUnsignedInteger i_value, const tUnsignedInteger i_multiple );
		// If the multiple is known to be a power-of-2 this is cheaper than the previous function
		template<typename tUnsignedInteger, class EnforceUnsigned = typename std::enable_if<std::is_unsigned<tUnsignedInteger>::value>::type>
			tUnsignedInteger RoundUpToMultiple_powerOf2( const tUnsignedInteger i_value, const tUnsignedInteger i_multipleWhichIsAPowerOf2 );
	}
}

#include "Functions.inl"

#endif	// EAE6320_MATH_FUNCTIONS_H
