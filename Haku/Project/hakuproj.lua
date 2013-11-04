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
	else
		kind "StaticLib"
	end

	-- setup paths --
	includedirs { "../../dist/include" }
	includedirs { "../../dist/include/Haku" }
	objdir "../Build"
	targetdir "../../dist/lib"

	-- add the source code --
	files { "../*.TXT" }

	files { "../../dist/include/Haku/**.h", "../../dist/include/Haku/**.inl" }
	files { "../Source/**.h", "../Source/**.cpp" }

	-- project configuration --

	flags { "StaticRuntime", "NoExceptions", "NoRTTI" }
	warnings "Extra"

--	pchheader "Haku.h"
--	pchsource "HKMain.cpp"

	-- configure standard fuji stuff --
	dofile "../../dist/Project/fujiconfig.lua"

	-- setup output directories --
	for i, p in pairs(platformNames) do
		configuration { i }
			targetdir("../../dist/lib/" .. iif(p, p .. "/", ""))
	end

	configuration "Debug"
		if os.get() == "windows" then
			targetsuffix ("_" .. configNames.Debug)
		else
			targetsuffix ("-debug")
		end

	configuration "DebugOpt"
		if os.get() == "windows" then
			targetsuffix ("_" .. configNames.DebugOpt)
		else
			targetsuffix ("-debugopt")
		end

	configuration "Release"
		if os.get() == "windows" then
			targetsuffix ("_" .. configNames.Release)
		else
			targetsuffix ("")
		end

	configuration "Retail"
		if os.get() == "windows" then
			targetsuffix ("_" .. configNames.Retail)
		else
			targetsuffix ("")
		end

	configuration { }
