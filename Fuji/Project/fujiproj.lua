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
		if os.is("linux") then
			-- linux shared libs append the version number AFTER the extension
			targetextension(".so." .. fujiVersion)
		end
	else
		kind "StaticLib"
		flags { "OmitDefaultLibrary" }
	end

	-- setup paths --
	includedirs { "../Source", "../../dist/include/Fuji" }
	objdir "../Build"

	-- add the source code --
	files { "../*.TXT" }

	files { "../../dist/include/Fuji/**.h", "../../dist/include/Fuji/**.inl" }
	files { "../Source/**.h", "../Source/**.c", "../Source/**.cpp", "../Source/**.inc" }

	-- include some middleware directly --
	includedirs { "../Middleware/" }
	files { "../Middleware/minizip/**.h", "../Middleware/minizip/**.c" }
	configuration { "not linux" }
		includedirs { "../Middleware/vorbis/include/", "../Middleware/libmad/" }

	if os.get() ~= "linux" then
		configuration { "not linux", "not macosx", "not Android" }
			files { "../Middleware/zlib/*.h", "../Middleware/zlib/*.c" }
			includedirs { "../Middleware/zlib" }
			files { "../Middleware/libpng-1.5.0/**.h", "../Middleware/libpng-1.5.0/**.c" }
			includedirs { "../Middleware/libpng-1.5.0/" }
		configuration { "not linux", "not macosx", "not Android", "not windows" }
			files { "../Middleware/libpng-1.5.0/**.h", "../Middleware/libpng-1.5.0/**.c" }
			includedirs { "../Middleware/libpng-1.5.0/" }
	end

	configuration { }

	-- project configuration --
	flags { "StaticRuntime", "NoExceptions", "NoRTTI" }
	warnings "Extra"

	if string.startswith(_ACTION, "vs") then
		linkoptions { "/ignore:4221" }
	end

	-- configure standard fuji stuff --
	dofile "../../dist/Project/fujiconfig.lua"

	-- setup output directories --
	dofile "outputdir.lua"

	-- platform specific config --
	configuration { "windows", "not Xbox360", "not PS3", "not Android" }
		excludes { "../Source/Images/**" }
	configuration { "SharedLib", "windows", "not Xbox360", "not PS3", "not Android" }
		links { "FujiMiddleware" }
	configuration { "StaticLib", "windows", "not Xbox360", "not PS3", "not Android" }
		targetname "Fuji_static"

	configuration { "linux" }
		includedirs { "../Source/Images/LINUX/" }
	configuration { "macosx" }
		includedirs { "../Source/Images/OSX/" }
	configuration { "windows", "not Xbox360", "not PS3", "not Android" }
		includedirs { "../Source/Images/WINDOWS/" }
	configuration { "Android" }
		includedirs { "../Source/Images/ANDROID/" }
	configuration { "Xbox" }
		includedirs { "../Source/Images/XBOX/" }
	configuration { "Xbox360" }
		includedirs { "../Source/Images/X360/" }
	configuration { "PS3" }
		includedirs { "../Source/Images/PS3/" }

	configuration { }
