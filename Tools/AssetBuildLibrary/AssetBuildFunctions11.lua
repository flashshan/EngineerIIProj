--[[
	This file contains the logic for building assets
]]

-- Static Data Initialization
--===========================

-- Environment Variables
local EngineSourceContentDir, GameInstallDir, GameLicenseDir, GameSourceContentDir, LicenseDir, OutputDir 
do
	-- EngineSourceContentDir
	do
		local errorMessage
		EngineSourceContentDir, errorMessage = GetEnvironmentVariable( "EngineSourceContentDir" )
		if not EngineSourceContentDir then
			error( errorMessage )
		end
	end
	-- GameInstallDir
	do
		local errorMessage
		GameInstallDir, errorMessage = GetEnvironmentVariable( "GameInstallDir" )
		if not GameInstallDir then
			error( errorMessage )
		end
	end
	-- GameLicenseDir
	do
		local errorMessage
		GameLicenseDir, errorMessage = GetEnvironmentVariable( "GameLicenseDir" )
		if not GameLicenseDir then
			error( errorMessage )
		end
	end
	-- GameSourceContentDir
	do
		local errorMessage
		GameSourceContentDir, errorMessage = GetEnvironmentVariable( "GameSourceContentDir" )
		if not GameSourceContentDir then
			error( errorMessage )
		end
	end
	-- LicenseDir
	do
		local errorMessage
		LicenseDir, errorMessage = GetEnvironmentVariable( "LicenseDir" )
		if not LicenseDir then
			error( errorMessage )
		end
	end
	-- OutputDir
	do
		local errorMessage
		OutputDir, errorMessage = GetEnvironmentVariable( "OutputDir" )
		if not OutputDir then
			error( errorMessage )
		end
	end
end

-- External Interface
--===================


function BuildAssets()
	local wereThereErrors = false

	-- Build the shaders and copy them to the installation location
	do
		local path_shaderBuilder = OutputDir .. "ShaderBuilder.exe"

		-- build default Vert shader
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/defaultVert.shader"
			local shader_built = GameInstallDir .. "data/Shaders/Vertex/defaultVert.shd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end

		-- build default Frag shader
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/defaultFrag.shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/defaultFrag.shd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end

		-- Build sprite vert shader
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/spriteVert.shader"
			local shader_built = GameInstallDir .. "data/Shaders/Vertex/spriteVert.shd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		-- Build mesh vert shader
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/meshVert.shader"
			local shader_built = GameInstallDir .. "data/Shaders/Vertex/meshVert.shd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end

		-- Build sprite Frag shader 1
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/spriteFrag1.shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/spriteFrag1.shd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		-- Build sprite Frag shader 2
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/spriteFrag2.shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/spriteFrag2.shd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		-- Build sprite Frag shader 3
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/spriteFrag3.shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/spriteFrag3.shd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		-- Build mesh Frag shader
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/meshFrag.shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/meshFrag.shd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end

		if EAE6320_PLATFORM_D3D then
			do
				local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/spriteInputLayout.hlsl"
				local shader_built = GameInstallDir .. "data/Shaders/Vertex/spriteInputLayout.shd"
				CreateDirectoryIfItDoesntExist( shader_built )
				local command = "\"" .. path_shaderBuilder .. "\""
					.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
				local result, exitCode = ExecuteCommand( command )
				if result then
					if exitCode == 0 then
						-- Display a message for each asset
						print( "Built " .. shader_authored )
					else
						wereThereErrors = true
						-- The builder should already output a descriptive error message if there was an error
						-- (remember that you write the builder code,
						-- and so if the build process failed it means that _your_ code has returned an error code)
						-- but it can be helpful to still return an additional vague error message here
						-- in case there is a bug in the specific builder that doesn't output an error message
						OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
					end
				else
					wereThereErrors = true
					-- If the command wasn't executed then the second return value is an error message
					OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
				end
			end
			do
				local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/meshInputLayout.hlsl"
				local shader_built = GameInstallDir .. "data/Shaders/Vertex/meshInputLayout.shd"
				CreateDirectoryIfItDoesntExist( shader_built )
				local command = "\"" .. path_shaderBuilder .. "\""
					.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
				local result, exitCode = ExecuteCommand( command )
				if result then
					if exitCode == 0 then
						-- Display a message for each asset
						print( "Built " .. shader_authored )
					else
						wereThereErrors = true
						-- The builder should already output a descriptive error message if there was an error
						-- (remember that you write the builder code,
						-- and so if the build process failed it means that _your_ code has returned an error code)
						-- but it can be helpful to still return an additional vague error message here
						-- in case there is a bug in the specific builder that doesn't output an error message
						OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
					end
				else
					wereThereErrors = true
					-- If the command wasn't executed then the second return value is an error message
					OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
				end
			end
		end
	end


	-- Build the texture and copy them to the installation location
	do
		local path_textureBuilder = OutputDir .. "TextureBuilder.exe"
		-- build texture1
		do
			local texture_authored = EngineSourceContentDir .. "Textures/texture1.png"
			local texture_built = GameInstallDir .. "data/Textures/texture1.tex"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end

		-- build texture2
		do
			local texture_authored = EngineSourceContentDir .. "Textures/texture2.png"
			local texture_built = GameInstallDir .. "data/Textures/texture2.tex"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end

		-- build texture3
		do
			local texture_authored = EngineSourceContentDir .. "Textures/texture3.png"
			local texture_built = GameInstallDir .. "data/Textures/texture3.tex"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end
		-- build texture3
		do
			local texture_authored = EngineSourceContentDir .. "Textures/rose.jpg"
			local texture_built = GameInstallDir .. "data/Textures/rose.tex"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end
	end

	-- Build the meshes from content to installation location
	do
		local path_meshBuilder = OutputDir .. "MeshBuilder.exe"
		-- build haze
		do
			local mesh_authored = EngineSourceContentDir .. "Meshes/roseMaya.mesh"
			local mesh_built = GameInstallDir .. "data/Meshes/roseMaya.mesh"
			CreateDirectoryIfItDoesntExist( mesh_built )
			local command = "\"" .. path_meshBuilder .. "\""
				.. " \"" .. mesh_authored .. "\" \"" .. mesh_built
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. mesh_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), mesh_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), mesh_authored )
			end
		end

		-- build plane
		do
			local mesh_authored = EngineSourceContentDir .. "Meshes/planeMaya.mesh"
			local mesh_built = GameInstallDir .. "data/Meshes/planeMaya.mesh"
			CreateDirectoryIfItDoesntExist( mesh_built )
			local command = "\"" .. path_meshBuilder .. "\""
				.. " \"" .. mesh_authored .. "\" \"" .. mesh_built
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. mesh_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), mesh_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), mesh_authored )
			end
		end	
	end	 

	-- Copy the licenses to the installation location
	do
		CreateDirectoryIfItDoesntExist( GameLicenseDir )
		local sourceLicenses = GetFilesInDirectory( LicenseDir )
		for i, sourceLicense in ipairs( sourceLicenses ) do
			local sourceFileName = sourceLicense:sub( #LicenseDir + 1 )
			local targetPath = GameLicenseDir .. sourceFileName
			local result, errorMessage = CopyFile( sourceLicense, targetPath )
			if result then
				-- Display a message
				print( "Installed " .. sourceFileName )
			else
				wereThereErrors = true
				OutputErrorMessage( "The license \"" .. sourceLicense .. "\" couldn't be copied to \"" .. targetPath .. "\": " .. errorMessage )
			end
		end
	end

	-- Copy the setting.ini to the installation location
	do
		CreateDirectoryIfItDoesntExist( GameInstallDir )
		local settingFile = "settings.ini"
		local sourcePath = OutputDir .. settingFile
		local targetPath = GameInstallDir .. settingFile
		local result, errorMessage = CopyFile( sourcePath, targetPath )
		if result then
			-- Display a message
			print( "Installed setting file" .. settingFile )
		else
			wereThereErrors = true
			OutputErrorMessage( "The setting file \"" .. settingFile .. "\" couldn't be copied to \"" .. targetPath .. "\": " .. errorMessage )
		end
	end

	return not wereThereErrors
end
