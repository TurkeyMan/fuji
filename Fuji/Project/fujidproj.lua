require "monodevelop"
require "d"

fujiVersion = "0.7.1"

project ("FujiD")
	language "D"
	kind "StaticLib"
	flags { "OmitDefaultLibrary" }

	-- setup paths --
	includedirs { "../../dist/include/d2" }
	objdir "../Build"
	targetdir "../../dist/lib"

	files { "../../dist/include/d2/**.d" }

	-- project configuration --

	flags { "StaticRuntime" }
	warnings "Extra"

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

