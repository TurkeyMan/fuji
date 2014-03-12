require "monodevelop"
require "d"

fujiVersion = "0.7.1"

project ("FujiD")
	language "D"
	kind "StaticLib"
	flags { "StaticRuntime", "OmitDefaultLibrary" }

	-- setup paths --
	includedirs { "../../dist/include/d2" }
	objdir "../Build"

	files { "../../dist/include/d2/**.d" }
	excludes { "../../dist/include/d2/haku/**.d" }

	-- project configuration --
	warnings "Extra"

	-- configure standard fuji stuff --
	dofile "../../dist/Project/fujiconfig.lua"

	-- setup output directories --
	dofile "outputdir.lua"

	configuration "Release"
		flags { "NoBoundsCheck" }
	configuration "Retail"
		flags { "NoBoundsCheck" }

	configuration { }
