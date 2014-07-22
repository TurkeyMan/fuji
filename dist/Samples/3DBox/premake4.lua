project "3DBox"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp" }

	objdir "Build/"
	targetdir "./"

	flags { "WinMain", "StaticRuntime", "NoExceptions", "NoRTTI" }
	warnings "Extra"

	links { "Fuji" }

	dofile "../../Project/fujiconfig.lua"

	configuration { "windows" }
		links { "FujiMiddleware.lib" }
