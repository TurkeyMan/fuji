project "ModelTest"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp" }

	objdir "Build/"
	targetdir "./"

	flags { "WinMain", "StaticRuntime", "NoExceptions", "NoRTTI" }
	warnings "Extra"

	links { "Fuji" }

	dofile "../../Project/fujiconfig.lua"
