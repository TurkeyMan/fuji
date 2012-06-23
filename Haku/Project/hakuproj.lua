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
	includedirs { "../Include/", "../Source/" }
	objdir "../Build/"
	targetdir "../Lib/"

	-- add the source code --
	files { "../*.TXT" }

	files { "../Include/**.h", "../Include/**.inl" }
	files { "../Source/**.h", "../Source/**.cpp" }

	-- project configuration --

	flags { "StaticRuntime", "NoExceptions", "NoRTTI", "ExtraWarnings" }

--	pchheader "Haku.h"
--	pchsource "HKMain.cpp"

	-- configure standard fuji stuff --
	dofile "../../Fuji/Public/Project/fujiconfig.lua"

	-- setup output directories --
	for i, p in pairs(platformNames) do
		configuration { i }
			targetdir("../../Public/Lib/" .. iif(p, p .. "/", ""))
	end

	configuration "Debug"
		if isVS() then
			targetsuffix ("_" .. configNames.Debug)
		else
			targetsuffix ("-debug-" .. hakuVersion)
		end

	configuration "DebugOpt"
		if isVS() then
			targetsuffix ("_" .. configNames.DebugOpt)
		else
			targetsuffix ("-debugopt-" .. hakuVersion)
		end

	configuration "Release"
		if isVS() then
			targetsuffix ("_" .. configNames.Release)
		else
			targetsuffix ("-" .. hakuVersion)
		end

	configuration "Retail"
		if isVS() then
			targetsuffix ("_" .. configNames.Retail)
		else
			targetsuffix ("-" .. hakuVersion)
		end

	configuration { }
