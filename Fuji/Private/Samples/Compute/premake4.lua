project "Compute"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp" }

	objdir "Build/"
	targetdir "./"

	flags { "StaticRuntime", "NoExceptions", "NoRTTI" }

	links { "Fuji" }

	dofile "../../Project/fujiconfig.lua"
