// Include Files
//==============

#include "cMeshBuilder.h"

#include <Engine/Platform/Platform.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <External/Lua/Includes.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Math/Functions.h>

//#include <Engine/Time/Time.h>
//#include <fstream> 

// Build
//------

eae6320::cResult eae6320::Assets::cNeshBuilder::Build(const std::vector<std::string>&)
{
	cResult result = Results::Success;
	// create directory
	{
		std::string errorMessage;
		if (!(result = eae6320::Platform::CreateDirectoryIfItDoesntExist(m_path_source, &errorMessage)))
		{
			OutputErrorMessageWithFileInfo(m_path_source, errorMessage.c_str());
			goto OnExit;
		}
	}

	// get data from lua file
	lua_State* luaState = nullptr;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			Assets::OutputErrorMessageWithFileInfo(m_path_source, "Failed to create a new Lua state");		
			goto OnExit;
		}
	}

	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, m_path_source);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			Assets::OutputErrorMessageWithFileInfo(m_path_source, lua_tostring(luaState, -1));
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	//into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					Assets::OutputErrorMessageWithFileInfo(m_path_source, "Asset files must return a table");
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				Assets::OutputErrorMessageWithFileInfo(m_path_source, "Asset files must return a single table");
				lua_pop(luaState, returnedValueCount);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			Assets::OutputErrorMessageWithFileInfo(m_path_source, lua_tostring(luaState, -1));
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	uint16_t vertexCount;
	Graphics::VertexFormats::sGeometry *vertexData;
	const char *key = "vertexData";
	lua_pushstring(luaState, key);
	lua_gettable(luaState, -2);
	if (lua_istable(luaState, -1))
	{
		vertexCount = static_cast<uint16_t>(luaL_len(luaState, -1));
		vertexData = new (std::nothrow) Graphics::VertexFormats::sGeometry[vertexCount];
		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			lua_pushinteger(luaState, i + 1);
			lua_gettable(luaState, -2);

			const char* tempKey = "position";
			lua_pushstring(luaState, tempKey);
			lua_gettable(luaState, -2);
			if (lua_istable(luaState, -1))
			{
				// get x
				lua_pushinteger(luaState, 1);
				lua_gettable(luaState, -2);
				vertexData[i].x = static_cast<float>(lua_tonumber(luaState, -1));
				lua_pop(luaState, 1);
				// get y
				lua_pushinteger(luaState, 2);
				lua_gettable(luaState, -2);
				vertexData[i].y = static_cast<float>(lua_tonumber(luaState, -1));
				lua_pop(luaState, 1);
				// get z
				lua_pushinteger(luaState, 3);
				lua_gettable(luaState, -2);
				vertexData[i].z = static_cast<float>(lua_tonumber(luaState, -1));
				lua_pop(luaState, 1);
			}
			else
			{
				result = Results::Failure;
				Assets::OutputErrorMessageWithFileInfo(m_path_source, "The mesh file has invalid vertex data position");
				goto OnExit;
			}
			lua_pop(luaState, 1);

			tempKey = "color";
			lua_pushstring(luaState, tempKey);
			lua_gettable(luaState, -2);
			if (lua_istable(luaState, -1))
			{
				// get r
				lua_pushinteger(luaState, 1);
				lua_gettable(luaState, -2);
				vertexData[i].r = Math::ClampColor(static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0 + 0.5), 0, 255);
				lua_pop(luaState, 1);
				// get g
				lua_pushinteger(luaState, 2);
				lua_gettable(luaState, -2);
				vertexData[i].g = Math::ClampColor(static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0 + 0.5), 0, 255);
				lua_pop(luaState, 1);
				// get b
				lua_pushinteger(luaState, 3);
				lua_gettable(luaState, -2);
				vertexData[i].b = Math::ClampColor(static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0 + 0.5), 0, 255);
				lua_pop(luaState, 1);
				// get a
				lua_pushinteger(luaState, 4);
				lua_gettable(luaState, -2);
				vertexData[i].a = Math::ClampColor(static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0 + 0.5), 0, 255);
				lua_pop(luaState, 1);
			}
			else
			{
				result = Results::Failure;
				Assets::OutputErrorMessageWithFileInfo(m_path_source, "The mesh file has invalid vertex data color");
				goto OnExit;
			}
			lua_pop(luaState, 1);

			tempKey = "uv";
			lua_pushstring(luaState, tempKey);
			lua_gettable(luaState, -2);
			if (lua_istable(luaState, -1))
			{
				// get u
				lua_pushinteger(luaState, 1);
				lua_gettable(luaState, -2);
				vertexData[i].u = static_cast<float>(lua_tonumber(luaState, -1));
				lua_pop(luaState, 1);
				// get v
				lua_pushinteger(luaState, 2);
				lua_gettable(luaState, -2);
#ifdef EAE6320_PLATFORM_D3D
				vertexData[i].v = 1.0f - static_cast<float>(lua_tonumber(luaState, -1));
#elif defined EAE6320_PLATFORM_GL
				vertexData[i].v = static_cast<float>(lua_tonumber(luaState, -1));
#endif
				lua_pop(luaState, 1);
			}
			else
			{
				result = Results::Failure;
				Assets::OutputErrorMessageWithFileInfo(m_path_source, "The mesh file has invalid vertex data uv");
				goto OnExit;
			}
			lua_pop(luaState, 1);

			lua_pop(luaState, 1);
		}
	}
	else
	{
		result = Results::Failure;
		Assets::OutputErrorMessageWithFileInfo(m_path_source, "The mesh file has invalid vertex data type");
		goto OnExit;
	}
	// pop vertexData
	lua_pop(luaState, 1);

	uint16_t indexCount;
	uint16_t *indexData;
	key = "indexData";
	lua_pushstring(luaState, key);
	lua_gettable(luaState, -2);
	if (lua_istable(luaState, -1))
	{
		indexCount = static_cast<uint16_t>(luaL_len(luaState, -1));
		indexData = new (std::nothrow) uint16_t[indexCount];
		for (unsigned int i = 0; i < indexCount; ++i)
		{
			lua_pushinteger(luaState, i + 1);
			lua_gettable(luaState, -2);

#ifdef EAE6320_PLATFORM_D3D
			indexData[indexCount - 1 - i] = static_cast<uint16_t>(lua_tointeger(luaState, -1));		
#elif defined EAE6320_PLATFORM_GL
			indexData[i] = static_cast<uint16_t>(lua_tointeger(luaState, -1));
#endif
			lua_pop(luaState, 1);
		}
	}
	else
	{
		result = Results::Failure;
		Assets::OutputErrorMessageWithFileInfo(m_path_source, "The mesh file has invalid index data type");
		goto OnExit;
	}
	// pop vertexData
	lua_pop(luaState, 1);

	// Pop the table
	lua_pop(luaState, 1);


	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = nullptr;
	}
	
	// Copy the file
	{
		std::string errorMessage;
		// There are many reasons that a source should be rebuilt,
		// and so even if the target already exists it should just be written over
		constexpr bool noErrorIfTargetAlreadyExists = false;
		// Since we rely on timestamps to determine when a target was built
		// its file time should be updated when the source gets copied
		constexpr bool updateTheTargetFileTime = true;
		
		FILE * pFile;
		pFile = fopen(m_path_target, "wb");

		fwrite(&vertexCount, sizeof(uint16_t), 1, pFile);
		fwrite(vertexData, sizeof(*vertexData), vertexCount, pFile);
		fwrite(&indexCount, sizeof(uint16_t), 1, pFile);
		fwrite(indexData, sizeof(*indexData), indexCount, pFile);

		fclose(pFile);
	}

	if (vertexData != nullptr)
		delete[] vertexData;
	if (indexData != nullptr)
		delete[] indexData;

OnExit:
	return result;
}
