project "UI"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp" }

	objdir "Build/"
	targetdir "./"

	staticruntime "On"
	warnings "Extra"
	exceptionhandling "Off"
	rtti "Off"

	links { "Fuji", "Haku" }

	dofile "../../Project/fujiconfig.lua"
	dofile "../../Project/hakuconfig.lua"

	configuration { "windows" }
		entrypoint "WinMainCRTStartup"
		links { "FujiMiddleware.lib" }
