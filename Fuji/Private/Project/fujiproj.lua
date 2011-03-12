includedirs { "../Include/", "../Middleware/" }
libdirs { "../Lib/" }

include "../Middleware/libjson/"
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
	files { "../Middleware/zlib/minizip/**.h", "../Middleware/zlib/minizip/**.c" }
--	files { "../Middleware/libjson/**.h", "../Middleware/libjson/**.cpp" }
--	files { "../Middleware/minifmod170/lib/**.h", "../Middleware/minifmod170/lib/**.c" }

	-- add source for some middleware that we'll link directly on unix systems --
	configuration { "not linux", "not macosx" }
	files { "../Middleware/zlib/*.h", "../Middleware/zlib/*.c" }
	files { "../Middleware/minifmod170/lib/**.h", "../Middleware/minifmod170/lib/**.c" }
	files { "../Middleware/libpng-1.5.0/**.h", "../Middleware/libpng-1.5.0/**.c" }
	includedirs { "../Middleware/libpng-1.5.0/" }
	configuration { }

	-- setup paths --
	includedirs { "../Source/" }
	includedirs { "../Middleware/zlib", "../Middleware/angelscript/include/", "../Middleware/vorbis/include/", "../Middleware/libmad/", "../Middleware/hqx/" }
	objdir "../Build/"
	targetdir "../Lib/"

	defines { "_LIB" }
	flags { "StaticRuntime", "NoExceptions", "NoRTTI" }

--	pchheader "Fuji.h"
--	pchsource "MFMain.cpp"

	configuration "../Middleware/libjson/**.cpp"
		flags { "StaticRuntime", "NoRTTI" }


	-- configure standard fuji stuff --

	dofile "fujiconfig.lua"


	-- platform specific config --

	-- Linux --
	configuration { "linux" }
		includedirs { "../Source/Images/LINUX/" }

	-- OSX --
	configuration { "macosx" }
		includedirs { "../Source/Images/OSX/" }

	-- Windows --
	configuration { "windows", "not Xbox360", "not PS3" }
		includedirs { "../Source/Images/WINDOWS/" }

	-- XBox --
	configuration "Xbox"
		includedirs { "../Source/Images/XBOX/" }

	-- XBox 360 --
	configuration "Xbox360"
		includedirs { "../Source/Images/X360/" }

	-- Playstation 3 --
	configuration "PS3"
		includedirs { "../Source/Images/PS3/" }

	configuration { }
