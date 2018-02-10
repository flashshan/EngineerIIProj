--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	meshes =
	{
		"Meshes/playerCube.mesh",
		"Meshes/enemyCube.mesh",
		"Meshes/backgroundPlane.mesh",
		"Meshes/bulletPlane.mesh",
	},

	shaders =
	{
		-- EAE6320_TODO:
		-- These are some examples showing how to use the optional argument,
		-- but with my disk layout and file extension.
		-- You need to update the paths to match your content
		-- and then add the other shaders that you use.
		{ path = "Shaders/Vertex/spriteInputLayout.shader", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/meshInputLayout.shader", arguments = { "vertex" } },
		
		{ path = "Shaders/Vertex/defaultVert.shader", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/spriteVert.shader", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/meshVert.shader", arguments = { "vertex" } },

		{ path = "Shaders/Fragment/defaultFrag.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/spriteFrag.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/spriteColorFrag.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/spriteTransFrag.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/meshFrag.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/meshTransFrag.shader", arguments = { "fragment" } },
	},

	textures =
	{
		-- EAE6320_TODO:
		-- These are some examples with my disk layout.
		-- You need to update the paths to match your content
		-- and then add the other textures that you use.
	

		"Textures/white.png",
		"Textures/red.png",
		"Textures/green.png",
		"Textures/blue.png",
		"Textures/yellow.png",

		"Textures/YouWin.png",
		"Textures/YouLose.png",
		"Textures/Restart.png",
		"Textures/Start.png",
	},
}

