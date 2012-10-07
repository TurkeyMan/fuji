project "InputTest"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp" }

	objdir "Build/"
	targetdir "./"

	flags { "WinMain", "StaticRuntime", "NoExceptions", "NoRTTI", "ExtraWarnings" }

	links { "Fuji" }

	dofile "../../Project/fujiconfig.lua"
