includedirs { "../Include/", "../Middleware/" }
libdirs { "../Lib/" }

include "../Middleware/libmad/"

project "Fuji"
	kind "StaticLib"
	language "C++"

	-- add the source code --
	files { "../*.TXT" }

	files { "../Include/**.h" }
	files { "../Source/**.h", "../Source/**.cpp", "../Source/**.inc" }
	excludes { "../Source/Images/**" }
	files { "../Util/**.h", "../Util/**.cpp" }

	-- include some middleware directly --
	files { "../Middleware/tinyxml/**.h", "../Middleware/tinyxml/**.cpp" }
--	files { "../Middleware/minifmod170/lib/**.h", "../Middleware/minifmod170/lib/**.c" }

	-- add source for some middleware that we'll link directly on unix systems --
	configuration { "not linux", "not macosx" }
	files { "../Middleware/zlib/**.h", "../Middleware/zlib/**.c" }
	files { "../Middleware/libpng-1.5.0/**.h", "../Middleware/libpng-1.5.0/**.c" }
	files { "../Middleware/libjson/**.h", "../Middleware/libjson/**.cpp" }
	files { "../Middleware/minifmod170/lib/**.h", "../Middleware/minifmod170/lib/**.c" }
	configuration { }

	-- setup paths --
	includedirs { "../Source/" }
	includedirs { "../Middleware/zlib", "../Middleware/angelscript/include/", "../Middleware/vorbis/include/", "../Middleware/libmad/", "../Middleware/libpng-1.5.0/", "../Middleware/hqx/" }
	objdir "../Build/"
	targetdir "../Lib/"

	defines { "_LIB" }
	flags { "StaticRuntime", "NoExceptions", "NoRTTI" }

--	pchheader "Fuji.h"
--	pchsource "MFMain.cpp"

	configuration "Debug"
		defines { "DEBUG", "_DEBUG" }
		flags { "Symbols" }
		targetsuffix "_Debug"

	configuration "DebugOpt"
		defines { "DEBUG", "_DEBUG" }
		flags { "Symbols", "OptimizeSpeed" }
		targetsuffix "_DebugOpt"

	configuration "Release"
		defines { "NDEBUG", "_RELEASE" }
		flags { "OptimizeSpeed" }
		targetsuffix "_Release"

	configuration "Retail"
		defines { "NDEBUG", "_RETAIL" }
		flags { "OptimizeSpeed" }
		targetsuffix "_Retail"


	-- platform specific config --

	-- Linux --
	configuration { "linux" }
		defines { "_LINUX" }
		includedirs { "../Source/Images/LINUX/" }
		links { "c", "m", "stdc++", "pthread", "GL", "GLU", "Xxf86vm", "ogg", "vorbis", "vorbisfile", "asound", "portaudio" }
		links { "zlib", "mad", "png", "json" }

	-- OSX --
	configuration { "macosx" }
		defines { "_OSX" }
		includedirs { "../Source/Images/OSX/" }
		links { "c", "m", "stdc++", "pthread", "GL", "GLU", "Xxf86vm", "X11", "ogg", "vorbis", "vorbisfile" }
		links { "zlib", "mad", "png", "json" }

	-- Windows --
	configuration { "windows", "not Xbox360", "not PS3" }
		defines { "WIN32", "_WINDOWS" }
		includedirs { "../Source/Images/WINDOWS/" }
		links { "ogg_static", "vorbisfile_static", "mad" }

		-- Windows 32 --
		configuration { "windows", "x32 or native", "not Xbox360", "not PS3" }
			libdirs { "../Middleware/vorbis/lib/x86", "../Middleware/lglcd/lib/x86" }
			
		-- Windows 64 --
		configuration { "windows", "x64", "not Xbox360", "not PS3" }
			libdirs { "../Middleware/vorbis/lib/x64", "../Middleware/lglcd/lib/x64" }

	-- XBox --
	configuration "Xbox"
		defines { "WIN32", "_XBOX" }
		includedirs { "../Source/Images/XBOX/" }
		libdirs { "../Middleware/vorbis/lib/xbox" }
		links { "mad" }

	-- XBox 360 --
	configuration "Xbox360"
		defines { "WIN32", "_XBOX" }
		includedirs { "../Source/Images/X360/" }
		links { "mad" }

	-- Playstation 3 --
	configuration "PS3"
		defines { "_PS3" }
		includedirs { "../Source/Images/PS3/" }
		links { "mad" }
