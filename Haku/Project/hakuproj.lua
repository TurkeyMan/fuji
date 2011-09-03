project "Haku"
	kind "StaticLib"
	language "C++"

	-- add the source code --
	files { "../*.TXT" }

	files { "../Include/**.h", "../Include/**.inl" }
	files { "../Source/**.h", "../Source/**.cpp" }

	-- setup paths --
	includedirs { "../Include/", "../Source/" }
	objdir "../Build/"
	targetdir "../Lib/"

	defines { "_LIB" }
	flags { "StaticRuntime", "NoExceptions", "NoRTTI", "ExtraWarnings" }

--	pchheader "Haku.h"
--	pchsource "HKMain.cpp"

	-- configure standard fuji stuff --

	dofile "../../Fuji/Public/Project/fujiconfig.lua"

	configuration { }
