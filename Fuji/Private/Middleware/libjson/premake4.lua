project "json"
	kind "StaticLib"
	language "C++"
	files { "**.cpp", "**.h" }

	targetdir "../../Lib/Middleware/libjson/"

	flags { "StaticRuntime", "NoRTTI" }

	defines { "_LIB", "_CRT_SECURE_NO_DEPRECATE" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		targetsuffix "_d"
	configuration "DebugOpt"
		defines { "DEBUG" }
		flags { "Symbols", "OptimizeSpeed" }
	configuration "Release"
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
	configuration "Retail"
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
