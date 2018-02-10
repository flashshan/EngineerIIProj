#ifndef EAE6320_APPLICATION_CBAPPLICATION_INL
#define EAE6320_APPLICATION_CBAPPLICATION_INL

// Include Files
//==============

#include "cbApplication.h"

// Interface
//==========

// Access
//-------

inline void eae6320::Application::cbApplication::SetSimulationRate( const float i_simulationRate )
{
	m_simulationRate = i_simulationRate;
}

#endif	// EAE6320_APPLICATION_CBAPPLICATION_INL
