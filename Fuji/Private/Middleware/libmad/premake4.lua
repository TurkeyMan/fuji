project "mad"
	kind "StaticLib"
	language "C"
	files { "**.c", "**.h", "**.dat" }
	excludes { "minimad.c" }

	includedirs { "msvc++" }
	targetdir "../../../Public/Lib/Middleware/libmad/"

	flags { "StaticRuntime", "NoExceptions", "NoRTTI" }

	defines { "_LIB", "HAVE_CONFIG_H", "ASO_ZEROCHECK", "_CRT_SECURE_NO_DEPRECATE" }

	configuration "Debug"
		defines { "DEBUG", "FPM_DEFAULT" }
		flags { "Symbols" }
		targetsuffix "_d"
	configuration "DebugOpt"
		defines { "DEBUG", "FPM_DEFAULT" }
		flags { "Symbols", "Optimize" }
	configuration "Release"
		defines { "NDEBUG", "FPM_INTEL" }
		flags { "OptimizeSpeed" }
	configuration "Retail"
		defines { "NDEBUG", "FPM_INTEL" }
		flags { "OptimizeSpeed" }
