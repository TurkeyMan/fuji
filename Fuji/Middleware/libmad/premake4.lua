project "mad"
	kind "StaticLib"
	language "C"
	files { "**.c", "**.h", "**.dat" }
	excludes { "minimad.c" }

	includedirs { "msvc++" }
	targetdir "../../../dist/lib/libmad/"

	flags { "StaticRuntime" }
	exceptionhandling "Off"
	rtti "Off"

	defines { "_LIB", "HAVE_CONFIG_H", "ASO_ZEROCHECK", "_CRT_SECURE_NO_DEPRECATE" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		targetsuffix "_d"
	configuration "DebugOpt"
		defines { "DEBUG" }
		flags { "Symbols", "Optimize" }
	configuration "Release"
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
	configuration "Retail"
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
