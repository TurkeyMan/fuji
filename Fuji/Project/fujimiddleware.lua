fujiVersion = "0.7.1"

project ("FujiMiddleware")
	language "C++"
	kind "SharedLib"
	defines { "MF_MIDDLEWARE", "MF_SHAREDLIB" }
	if os.get() == "linux" then
		-- linux shared libs append the version number AFTER the extension
		targetextension(".so." .. fujiVersion)
	end

	-- setup paths --
	includedirs { "../Source", "../../dist/include", "../../dist/include/Fuji" }
	objdir "../Build"

	files { "../Source/Asset/Util/HLSL2GLSL.cpp", "../../dist/include/Fuji/Asset/Util/HLSL2GLSL.h" }

	-- include some middleware directly --
	includedirs { "../Middleware/" }

	-- project configuration --
	flags { "StaticRuntime", "NoExceptions", "NoRTTI" }
	warnings "Extra"

	-- configure standard fuji stuff --
	configuration { "Debug" }
		defines { "DEBUG", "_DEBUG" }
		flags { "Symbols" }
		optimize "Debug"

	configuration { "DebugOpt" }
		defines { "DEBUG", "_DEBUG" }
		flags { "Symbols" }
		optimize "On"

	configuration { "Release" }
		defines { "NDEBUG", "_RELEASE" }
		optimize "Full"

	configuration { "Retail" }
		defines { "NDEBUG", "_RETAIL" }
		optimize "Full"

	-- platforms --
	configuration { "linux" }
		if os.is64bit() then
			libdirs { "../../dist/lib/linux-x86_64" }
		else
			libdirs { "../../dist/lib/linux-i386" }
		end
		links { "hlsl2glsl", "glsl_optimizer" }

	configuration { "macosx" }
		libdirs { "../../dist/lib/osx" }
		links { "hlsl2glsl", "glsl_optimizer" }

	configuration { "windows", "not Xbox360", "not PS3", "not Android" }
		defines { "WIN32" }

		-- Windows 32 --
		configuration { "windows", "x32 or native", "not Xbox360", "not PS3", "not Android" }
			libdirs { "../../dist/lib/x86" }

		-- Windows 64 --
		configuration { "windows", "x64", "not Xbox360", "not PS3", "not Android" }
			libdirs { "../../dist/lib/x64" }

	-- setup output directories --
	dofile "outputdir.lua"
