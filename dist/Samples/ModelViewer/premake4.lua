project "ModelViewer"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp" }

	objdir "Build/"
	targetdir "./"

	flags { "WinMain", "StaticRuntime" }
	warnings "Extra"
	exceptionhandling "Off"
	rtti "Off"

	links { "Fuji" }

	dofile "../../Project/fujiconfig.lua"

	configuration { "windows" }
		links { "FujiMiddleware.lib" }
