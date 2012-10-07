-- add dependent projects
if _OS ~= "linux" then
	include "../Middleware/libmad/"
end

fujiVersion = "0.7.1"

projName = "Fuji"

if fujiDll == true then
	projName = "FujiDLL"
end

project (projName)
	language "C++"
	if fujiDll == true then
		kind "SharedLib"
		targetname "Fuji"
		defines { "MF_SHAREDLIB" }
	else
		kind "StaticLib"
	end

	-- setup paths --
	includedirs { "../Source", "../../dist/include/Fuji" }
--	libdirs { "../../dist/lib" }
	objdir "../Build"
	targetdir "../../dist/lib"

	-- add the source code --
	files { "../*.TXT" }

	files { "../../dist/include/Fuji/**.h", "../../dist/include/Fuji/**.inl" }
	files { "../Source/**.h", "../Source/**.c", "../Source/**.cpp", "../Source/**.inc" }
	excludes { "../Source/Images/**" }

	-- include some middleware directly --
	files { "../Middleware/zlib/minizip/**.h", "../Middleware/zlib/minizip/**.c" }
	includedirs { "../Middleware/zlib" }
	configuration { "not linux" }
		includedirs { "../Middleware/vorbis/include/", "../Middleware/libmad/" }
	if os.get() ~= "linux" then
	configuration { "not linux", "not macosx", "not Android" }
		files { "../Middleware/zlib/*.h", "../Middleware/zlib/*.c" }
		includedirs { "../Middleware/zlib" }
		files { "../Middleware/libpng-1.5.0/**.h", "../Middleware/libpng-1.5.0/**.c" }
		includedirs { "../Middleware/libpng-1.5.0/" }
		files { "../Middleware/minifmod170/lib/**.h", "../Middleware/minifmod170/lib/**.c" }
	end
	configuration { "windows", "not Xbox360", "not PS3", "not Android" }
		includedirs { "../Middleware/" }
	configuration { }

	-- project configuration --

	flags { "StaticRuntime", "NoExceptions", "NoRTTI", "ExtraWarnings" }

--	pchheader "Fuji.h"
--	pchsource "MFMain.cpp"

	-- configure standard fuji stuff --
	dofile "../../dist/Project/fujiconfig.lua"

	-- setup output directories --
	for i, p in pairs(platformNames) do
		configuration { i }
			targetdir("../../dist/lib/" .. iif(p, p .. "/", ""))
	end

	configuration "Debug"
		if isVS() then
			targetsuffix ("_" .. configNames.Debug)
		else
			targetsuffix ("-debug-" .. fujiVersion)
		end

	configuration "DebugOpt"
		if isVS() then
			targetsuffix ("_" .. configNames.DebugOpt)
		else
			targetsuffix ("-debugopt-" .. fujiVersion)
		end

	configuration "Release"
		if isVS() then
			targetsuffix ("_" .. configNames.Release)
		else
			targetsuffix ("-" .. fujiVersion)
		end

	configuration "Retail"
		if isVS() then
			targetsuffix ("_" .. configNames.Retail)
		else
			targetsuffix ("-" .. fujiVersion)
		end


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
