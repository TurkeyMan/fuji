hakuVersion = "0.1.1"

projName = "Haku"

if hakuDll == true then
	projName = "HakuDLL"
end

project (projName)
	language "C++"
	if hakuDll == true then
		kind "SharedLib"
		targetname "Haku"
		defines { "MF_SHAREDLIB" }
		if os.is("linux") then
			-- linux shared libs append the version number AFTER the extension
			targetextension(".so." .. fujiVersion)
		end
		flags { "StaticRuntime" }
	else
		kind "StaticLib"
		flags { "OmitDefaultLibrary" }
	end

	-- setup paths --
	includedirs { "../../dist/include", "../../dist/include/Haku" }
	objdir "../Build"

	-- add the source code --
	files { "../*.TXT" }

	files { "../../dist/include/Haku/**.h", "../../dist/include/Haku/**.inl" }
	files { "../Source/**.h", "../Source/**.cpp" }

	-- project configuration --
	warnings "Extra"
	exceptionhandling "Off"
	rtti "Off"

--	pchheader "Haku.h"
--	pchsource "HKMain.cpp"

	-- configure standard fuji stuff --
	dofile "../../dist/Project/fujiconfig.lua"

	-- setup output directories --
	dofile "../../Fuji/Project/outputdir.lua"
