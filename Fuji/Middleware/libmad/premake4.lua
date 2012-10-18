project "mad"
	kind "StaticLib"
	language "C"
	files { "**.c", "**.h", "**.dat" }
	excludes { "minimad.c" }

	includedirs { "msvc++" }
	targetdir "../../../dist/lib/libmad/"

	-- setup output directories --
	for i, p in pairs(platformNames) do
		configuration { i }
			targetdir("../../dist/lib/" .. iif(p, p .. "/", ""))
	end

	flags { "StaticRuntime", "NoExceptions", "NoRTTI" }

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
