#ifndef EAE6320_CVIEW_H
#define EAE6320_CVIEW_H

namespace eae6320
{
	namespace Graphics
	{
		struct sInitializationParameters;
	}
}

// Include Files
//==============

#include <Engine/Results/Results.h>


namespace eae6320
{
	namespace Graphics
	{
		class cView
		{
			// Interface
			//==========
		public:
			cResult Initialize(const sInitializationParameters& i_initializationParameters);
			void ClearView(float i_r, float i_g, float i_b, float i_a);
			void ClearDepth(float i_depth = 1.0f);

			void SwapBuffer();
			void CleanUp();
		};
	}
}

#endif	// EAE6320_CVIEW_H
