--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	meshes =
	{
		"Meshes/planeVertMaya.mesh",
		-- "Meshes/roseMaya.mesh",
		-- "Meshes/circleMaya.mesh",
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
		"Textures/picture11.png",
		"Textures/picture12.png",
		"Textures/picture13.png",
		"Textures/picture21.png",
		"Textures/picture22.png",
		"Textures/picture23.png",
		"Textures/picture31.png",
		"Textures/picture32.png",
		"Textures/picture33.png",

		"Textures/white.png",
		"Textures/YouWin.png",
		"Textures/Restart.png",
		"Textures/Start.png",
	},
}

