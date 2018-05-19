project "ModelViewer"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp" }

	objdir "Build/"
	targetdir "./"

	staticruntime "On"
	warnings "Extra"
	exceptionhandling "Off"
	rtti "Off"

	links { "Fuji" }

	dofile "../../Project/fujiconfig.lua"

	configuration { "windows" }
		entrypoint "WinMainCRTStartup"
		links { "FujiMiddleware.lib" }
