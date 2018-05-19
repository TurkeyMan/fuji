fujiVersion = "0.7.1"

project ("FujiD")
	language "C++"
	kind "StaticLib"
	staticruntime "On"
--	flags { "OmitDefaultLibrary" }

	-- setup paths --
	objdir "../Build"

	files { "../../dist/include/d2/**.d" }
	excludes { "../../dist/include/d2/haku/**.d" }

	-- project configuration --
	warnings "Extra"

	-- configure standard fuji stuff --
	dofile "../../dist/Project/fujiconfig.lua"

	-- setup output directories --
	dofile "outputdir.lua"
