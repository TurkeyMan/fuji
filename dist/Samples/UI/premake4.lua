project "UI"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp" }

	objdir "Build/"
	targetdir "./"

	flags { "WinMain", "StaticRuntime", "NoExceptions", "NoRTTI" }
	warnings "Extra"

	links { "Fuji", "Haku" }

	dofile "../../Project/fujiconfig.lua"
	dofile "../../Project/hakuconfig.lua"
