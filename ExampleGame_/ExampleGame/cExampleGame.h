/*
	This class is your specific game
*/

#ifndef EAE6320_CEXAMPLEGAME_H
#define EAE6320_CEXAMPLEGAME_H

// Include Files
//==============

#include "cCamera.h"

#include <Engine/Application/cbApplication.h>
#include <Engine/Results/Results.h>
#include <Engine/Math/cMatrix_transformation.h>

#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Physics/sRigidBodyState.h>

#include <vector>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "Resource Files/Resource.h"
#endif

//namespace eae6320
//{
//	namespace Graphics
//	{
//		class cEffect;
//		class cSprite;
//		namespace VertexFormats
//		{
//			struct sSprite;
//			struct sGeometry;
//		}
//	}
//}


// Class Declaration
//==================

namespace eae6320
{
	class cExampleGame : public Application::cbApplication
	{
		enum class GameState
		{
			GameRun,
			GameStop,
		};

		// Inherited Implementation
		//=========================

	private:

		// Configuration
		//--------------

#if defined( EAE6320_PLATFORM_WINDOWS )
		// The main window's name will be displayed as its caption (the text that is displayed in the title bar).
		// You can make it anything that you want, but please keep the platform name and debug configuration at the end
		// so that it's easy to tell at a glance what kind of build is running.
		virtual const char* GetMainWindowName() const override
		{
			return "ShanShuo's practice"
				" -- "
#if defined( EAE6320_PLATFORM_D3D )
				"Direct3D"
#elif defined( EAE6320_PLATFORM_GL )
				"OpenGL"
#endif
#ifdef _DEBUG
				" -- Debug"
#endif
				;
		}
		// Window classes are almost always identified by name;
		// there is a unique "ATOM" associated with them,
		// but in practice Windows expects to use the class name as an identifier.
		// If you don't change the name below
		// your program could conceivably have problems if it were run at the same time on the same computer
		// as one of your classmate's.
		// You don't need to worry about this for our class,
		// but if you ever ship a real project using this code as a base you should set this to something unique
		// (a generated GUID would be fine since this string is never seen)
		virtual const char* GetMainWindowClassName() const override { return "Example Shader Game"; }
		// The following three icons are provided:
		//	* IDI_EAEGAMEPAD
		//	* IDI_EAEALIEN
		//	* IDI_VSDEFAULT_LARGE / IDI_VSDEFAULT_SMALL
		// If you want to try creating your own a convenient website that will help is: http://icoconvert.com/
		virtual const WORD* GetLargeIconId() const override { static constexpr WORD iconId_large = IDI_VSDEFAULT_LARGE; return &iconId_large; }
		virtual const WORD* GetSmallIconId() const override { static constexpr WORD iconId_small = IDI_VSDEFAULT_SMALL; return &iconId_small; }
#endif

		virtual void SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate);

		// Run
		//----
		virtual void UpdateBasedOnInput() override;
		virtual void UpdateSimulationBasedOnInput() override;	

		virtual void UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate);

		// Initialization / Clean Up
		//--------------------------

		virtual cResult Initialize() override;
		virtual cResult CleanUp() override;

		// function about background
		void SetBackgroundColor(float i_r, float i_g, float i_b, float i_a);

		// functions about user defined data
		void InitAllData();
		void ClearAllData();

		// function about sprite and effect 
		cResult InitEffectAndSpritePairs();
		cResult ClearEffectAndSpritePairs();
		
		// function about mesh and effect
		cResult InitEffectAndMeshPairs();
		cResult ClearEffectAndMeshPairs();

		// function about texture
		cResult InitTextures();
		cResult ClearTextures();

		// function about mesh
		cResult InitMeshes();
		cResult ClearMeshes();

	private:
		// Game Logic
		void GameStart();
		void GameWin();
		bool CheckPuzzleState();
		void UpdateMeshPosition();

		std::vector<int> puzzleState_;
		Math::sVector piecePosition_[9];
		int currentWhitePosition_;

		GameState currentGameState_;

	private:
		// parameter for background
		float backgroundColor_[4];
		float tempBackgroundColor_[4];

		// container for sprite, mesh, effect and texture
		//std::vector<std::pair<Graphics::cEffect *, Graphics::cSprite *>> spritePairContainer_;
		//std::vector<std::pair<Graphics::cEffect *, Graphics::cMesh *>> meshPairContainer_;
		std::vector<Graphics::SpriteDataGroup> spriteDataGroups_;
		std::vector<Graphics::SpriteDataGroup> tempSpriteDataGroups_;
		std::vector<Graphics::MeshDataGroup> opaqueMeshDataGroups_;
		std::vector<Graphics::MeshDataGroup> transMeshDataGroups_;

		std::vector<std::pair<bool, Graphics::cTexture::Handle>> textureHandleContainer_;
		std::vector<std::pair<bool, Graphics::cMesh::Handle>> meshHandleContainer_;
		
		//std::vector<Graphics::cTexture *> spriteTextures_; 
		//std::vector<Graphics::cTexture *> opaqueMeshTextures_;
		//std::vector<Graphics::cTexture *> transMeshTextures_;

		// container to store user's data
		std::vector<const char *> vertShaderPaths_;
		std::vector<const char *> fragShaderPaths_;

		std::vector<int> spriteVertexCount_;
		std::vector<Graphics::VertexFormats::sSprite *> spriteVertexData_;

		// parameter for update
		float simulationTimeSinceLastFrame_ = 0.0f;

		// for move and acceleration
		//std::vector<std::pair<Math::sVector, Math::cQuaternion>> meshData_;
		/*Physics::sRigidBodyState simulatedRigidBody;
		Physics::sRigidBodyState opaqueStaticRigidBody;
		Physics::sRigidBodyState transStaticRigidBody1;
		Physics::sRigidBodyState transStaticRigidBody2;*/

		// for sprite 
		unsigned int totalSprites_;


		// for Camera
		cCamera mainCamera_;
		
		// for single key press
		bool rPressState_ = false;
		bool upPressState_ = false;
		bool downPressState_ = false;
		bool leftPressState_ = false;
		bool rightPressState_ = false;


		// parameters used in backgroundColor
		float totalSimuTime_;
	};
}

// Result Definitions
//===================

namespace eae6320
{
	namespace Results
	{
		namespace Application
		{
			// You can add specific results for your game here:
			//	* The System should always be Application
			//	* The __LINE__ macro is used to make sure that every result has a unique ID.
			//		That means, however, that all results _must_ be defined in this single file
			//		or else you could have two different ones with equal IDs.
			//	* Note that you can define multiple Success codes.
			//		This can be used if the caller may want to know more about how a function succeeded.
			constexpr cResult ExampleResult( IsFailure, System::Application, __LINE__, Severity::Default );
		}
	}
}

#endif	// EAE6320_CEXAMPLEGAME_H
