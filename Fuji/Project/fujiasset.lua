fujiVersion = "0.7.1"

project ("FujiAsset")
	language "C++"
	kind "SharedLib"
	defines { "MF_SHAREDLIB" }
	if os.get() == "linux" then
		-- linux shared libs append the version number AFTER the extension
		targetextension(".so." .. fujiVersion)
	end

	-- setup linker --
	links { "Fuji" }

	-- setup paths --
	includedirs { "../Source", "../../dist/include/Fuji" }
	objdir "../Build"

	-- add the source code --
	files { "../*.TXT" }

	files { "../../dist/include/Fuji/**.h", "../../dist/include/Fuji/**.inl" }
	files { "../Source/Asset/**", "../Source/Util/**" }

	-- include some middleware directly --
	includedirs { "../Middleware/" }
	configuration { "not linux" }
		includedirs { "../Middleware/vorbis/include/", "../Middleware/libmad/" }
	if os.get() ~= "linux" then
		configuration { "not linux", "not macosx", "not Android" }
			includedirs { "../Middleware/zlib" }
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

--	pchheader "Fuji.h"
--	pchsource "MFMain.cpp"

	-- some additional stuff for asset conversion
	configuration { "linux" }
		links { "z", "png" }
		links { "assimp", "hlsl2glsl", "glsl_optimizer" }

	configuration { "windows", "not Xbox360", "not PS3", "not Android" }
		if string.startswith(_ACTION, "vs") then
			configuration { "SharedLib", "windows", "not Xbox360", "not PS3", "not Android" }
				linkoptions { "/DelayLoad:d3dx11_43.dll" }		-- D3D11
				linkoptions { "/DelayLoad:D3DX9_43.dll" }		-- D3D9

				configuration { "SharedLib", "windows", "x32 or native", "not Xbox360", "not PS3", "not Android" }
					targetname "FujiAsset32"
					linkoptions { "/DelayLoad:Assimp32.dll" }							-- Assimp

				configuration { "SharedLib", "windows", "x64", "not Xbox360", "not PS3", "not Android" }
					targetname "FujiAsset64"
					linkoptions { "/DelayLoad:Assimp64.dll" }							-- Assimp
		end

	-- configure standard fuji stuff --
	dofile "../../dist/Project/fujiconfig.lua"

	-- setup output directories --
	dofile "outputdir.lua"
