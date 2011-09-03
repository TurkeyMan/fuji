includedirs { "../../Public/Include/", "../Middleware/" }
libdirs { "../../Public/Lib/" }

include "../Middleware/libjson/"
include "../Middleware/libmad/"

project "Fuji"
	kind "StaticLib"
	language "C++"

	-- add the source code --
	files { "../*.TXT" }

	files { "../../Public/Include/**.h", "../../Public/Include/**.inl" }
	files { "../Source/**.h", "../Source/**.cpp", "../Source/**.inc" }
	excludes { "../Source/Images/**" }

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
	targetdir "../../Public/Lib/"

	defines { "_LIB" }
	flags { "StaticRuntime", "NoExceptions", "NoRTTI", "ExtraWarnings" }

--	pchheader "Fuji.h"
--	pchsource "MFMain.cpp"

	configuration "../Middleware/libjson/**.cpp"
		flags { "StaticRuntime", "NoRTTI" }


	-- configure standard fuji stuff --

	dofile "../../Public/Project/fujiconfig.lua"


	-- platform specific config --

	-- Linux --
	configuration { "linux" }
		includedirs { "../Source/Images/LINUX/" }

	-- OSX --
	configuration { "macosx" }
		includedirs { "../Source/Images/OSX/" }

	-- Windows --
	configuration { "windows", "not Xbox360", "not PS3", "not Android" }
		includedirs { "../Source/Images/WINDOWS/" }

	-- Android --
	configuration { "Android" }
		includedirs { "../Source/Images/ANDROID/" }

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
