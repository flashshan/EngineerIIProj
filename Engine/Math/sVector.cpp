// Include Files
//==============

#include "sVector.h"

#include <cmath>
#include <Engine/Asserts/Asserts.h>

// Static Data Initialization
//===========================

namespace
{
	constexpr auto s_epsilon = 1.0e-9f;
}

// Interface
//==========

// Addition
//---------

eae6320::Math::sVector eae6320::Math::sVector::operator +( const sVector& i_rhs ) const
{
	return sVector( x + i_rhs.x, y + i_rhs.y, z + i_rhs.z );
}

eae6320::Math::sVector& eae6320::Math::sVector::operator +=( const sVector& i_rhs )
{
	x += i_rhs.x;
	y += i_rhs.y;
	z += i_rhs.z;
	return *this;
}

// Subtraction / Negation
//-----------------------

eae6320::Math::sVector eae6320::Math::sVector::operator -( const sVector& i_rhs ) const
{
	return sVector( x - i_rhs.x, y - i_rhs.y, z - i_rhs.z );
}

eae6320::Math::sVector& eae6320::Math::sVector::operator -=( const sVector& i_rhs )
{
	x -= i_rhs.x;
	y -= i_rhs.y;
	z -= i_rhs.z;
	return *this;
}

eae6320::Math::sVector eae6320::Math::sVector::operator -() const
{
	return sVector( -x, -y, -z );
}

// Multiplication
//---------------

eae6320::Math::sVector eae6320::Math::sVector::operator *( const float i_rhs ) const
{
	return sVector( x * i_rhs, y * i_rhs, z * i_rhs );
}

eae6320::Math::sVector& eae6320::Math::sVector::operator *=( const float i_rhs )
{
	x *= i_rhs;
	y *= i_rhs;
	z *= i_rhs;
	return *this;
}

// Division
//---------

eae6320::Math::sVector eae6320::Math::sVector::operator /( const float i_rhs ) const
{
	EAE6320_ASSERTF( std::abs( i_rhs ) > s_epsilon, "Can't divide by zero" );
	const auto rhs_reciprocal = 1.0f / i_rhs;
	return sVector( x * rhs_reciprocal, y * rhs_reciprocal, z * rhs_reciprocal );
}

eae6320::Math::sVector& eae6320::Math::sVector::operator /=( const float i_rhs )
{
	EAE6320_ASSERTF( std::abs( i_rhs ) > s_epsilon, "Can't divide by zero" );
	const auto rhs_reciprocal = 1.0f / i_rhs;
	x *= rhs_reciprocal;
	y *= rhs_reciprocal;
	z *= rhs_reciprocal;
	return *this;
}

// Length / Normalization
//-----------------------

float eae6320::Math::sVector::GetLength() const
{
	return std::sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
}

float eae6320::Math::sVector::Normalize()
{
	const auto length = GetLength();
	EAE6320_ASSERTF( length > s_epsilon, "Can't divide by zero" );
	operator /=( length );
	return length;
}

eae6320::Math::sVector eae6320::Math::sVector::GetNormalized() const
{
	const auto length = GetLength();
	EAE6320_ASSERTF( length > s_epsilon, "Can't divide by zero" );
	const auto length_reciprocal = 1.0f / length;
	return sVector( x * length_reciprocal, y * length_reciprocal, z * length_reciprocal );
}

// Comparison
//-----------

bool eae6320::Math::sVector::operator ==( const sVector& i_rhs ) const
{
	// Use & rather than && to prevent branches (all three comparisons will be evaluated)
	return ( x == i_rhs.x ) & ( y == i_rhs.y ) & ( z == i_rhs.z );
}

bool eae6320::Math::sVector::operator !=( const sVector& i_rhs ) const
{
	// Use | rather than || to prevent branches (all three comparisons will be evaluated)
	return ( x != i_rhs.x ) | ( y != i_rhs.y ) | ( z != i_rhs.z );
}

// Products
//---------

float eae6320::Math::Dot( const sVector& i_lhs, const sVector& i_rhs )
{
	return ( i_lhs.x * i_rhs.x ) + ( i_lhs.y * i_rhs.y ) + ( i_lhs.z * i_rhs.z );
}

eae6320::Math::sVector eae6320::Math::Cross( const sVector& i_lhs, const sVector& i_rhs )
{
	return sVector(
		( i_lhs.y * i_rhs.z ) - ( i_lhs.z * i_rhs.y ),
		( i_lhs.z * i_rhs.x ) - ( i_lhs.x * i_rhs.z ),
		( i_lhs.x * i_rhs.y ) - ( i_lhs.y * i_rhs.x )
	);
}
