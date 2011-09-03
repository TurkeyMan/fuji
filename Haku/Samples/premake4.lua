solution "Samples"
	configurations { "Debug", "DebugOpt", "Release", "Retail" }
	platforms { "Native", "x32", "x64" }

	-- include the Fuji project...
	dofile "../../Fuji/Private/Project/fujiproj.lua"

	-- include the Haku project...
	dofile "../Project/hakuproj.lua"

	-- include each of the samples --
	include "UI"
