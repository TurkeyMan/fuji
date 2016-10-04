-- some code to take advantage of visual studios project macros

function isVS()
	return _ACTION == "vs2013" or _ACTION == "vs2012" or _ACTION == "vs2010" or _ACTION == "vs2008" or _ACTION == "vs2005" or _ACTION == "vs2003" or _ACTION == "vs2002"
end

local function isModernVS()
	return _ACTION == "vs2012" or _ACTION == "vs2013"
end

local function getConfigName(configName)
	if _ACTION == "vs2010" or _ACTION == "vs2012" or _ACTION == "vs2013" then
		return "$(Configuration)"
	elseif _ACTION == "vs2008" or _ACTION == "vs2005" or _ACTION == "vs2003" or _ACTION == "vs2002" then
		return "$(ConfigurationName)"
	end
	return configName
end

configNames = {}
configNames.Debug = getConfigName("debug")
configNames.DebugOpt = getConfigName("debugopt")
configNames.Release = getConfigName("release")
configNames.Retail = getConfigName("retail")

platformNames = {}
--platformNames.Native = iif(isVS(), "$(Platform)", "")
platformNames.Native = ""
--platformNames.x32 = iif(isVS(), "$(Platform)", "x32")
platformNames.x32 = "x32"
--platformNames.x64 = iif(isVS(), "$(Platform)", "x64")
platformNames.x64 = "x64"
--platformNames.Android = iif(isVS(), "$(Platform)", "Android")
--platformNames.iOS = iif(isVS(), "$(Platform)", "iOS")
--platformNames.PS2 = iif(isVS(), "$(Platform)", "PS2")
platformNames.PS3 = iif(isVS(), "$(Platform)", "PS3")
--platformNames.PSP = iif(isVS(), "$(Platform)", "PSP")
--platformNames.XBox = iif(isVS(), "$(Platform)", "XBox")
platformNames.Xbox360 = iif(isVS(), "$(Platform)", "XBox360")

-- configurations --

configuration { }

includedirs { "../include/" }
importdirs { "../include/d2/" }

if os.get() == "windows" then
	separator = "_"
else
	separator = "-"
end

configuration { "Debug" }
	defines { "DEBUG", "_DEBUG" }
	symbols "On"
	optimize "Off"
	boundschecking "On"
	targetsuffix (separator .. configNames.Debug)

configuration { "DebugOpt" }
	defines { "DEBUG", "_DEBUG" }
	symbols "On"
	optimize "On"
	boundschecking "On"
	targetsuffix (separator .. configNames.DebugOpt)

configuration { "Release" }
	defines { "NDEBUG", "_RELEASE" }
	optimize "Full"
	boundschecking "Off"
	configuration { "Release", "not linux" }
		targetsuffix (separator .. configNames.Release)	-- Note: Maybe this should be Retail instead?

configuration { "Retail" }
	defines { "NDEBUG", "_RETAIL" }
	optimize "Full"
	boundschecking "Off"
	targetsuffix (separator .. configNames.Retail)



-- platform specific config --

-- Linux --
configuration { "not StaticLib", "linux" }
	links { "c", "m", "stdc++", "pthread", "z" }
	links { "mad", "ogg", "vorbis", "vorbisfile" }

	-- driver libs
	links { "Xxf86vm", "X11" }
--	links { "SDL2", "SDL2_gfx" }
	links { "GL", "GLU" }
	links { "openal" }

	-- asset libs
	links { "png" }
	links { "assimp", "hlsl2glsl", "glsl_optimizer", "glcpp-library", "mesa" }

	if os.is64bit() then
		libdirs { "../lib/linux-x86_64" }
	else
		libdirs { "../lib/linux-i386" }
	end

-- OSX --
configuration { "not StaticLib", "macosx" }
	links { "c", "m", "stdc++", "pthread", "z" }
	links { "mad", "ogg", "vorbis", "vorbisfile" }

	-- driver libs
	links { "Xxf86vm", "X11", "GL", "GLU" }
	links { "openal" }

	-- asset libs
	links { "png" }
	links { "assimp", "hlsl2glsl", "glsl_optimizer" }

	libdirs { "../lib/osx" }

-- Windows --
configuration { "windows", "not Xbox360", "not PS3", "not Android" }
	defines { "WIN32" }

	-- Windows 32 --
	configuration { "windows", "x32 or native", "not Xbox360", "not PS3", "not Android" }
		libdirs { "../lib/x86" }

	-- Windows 64 --
	configuration { "windows", "x64", "not Xbox360", "not PS3", "not Android" }
		libdirs { "../lib/x64" }

	if string.startswith(_ACTION, "vs") then
		configuration { "not StaticLib", "windows", "not Xbox360", "not PS3", "not Android" }
			linkoptions { "/Delay:unload" }
			linkoptions { "/DelayLoad:d3d11.dll" }			-- D3D11
			linkoptions { "/DelayLoad:d3d9.dll" }			-- D3D9
			linkoptions { "/DelayLoad:opengl32.dll" }		-- OpenGL
			linkoptions { "/DelayLoad:dsound.dll" }			-- Sound
			linkoptions { "/DelayLoad:dinput8.dll" }		-- DirectInput
--			linkoptions { "/DelayLoad:xinput1_3.dll" }		-- XInput
			linkoptions { "/DelayLoad:ws2_32.dll" }			-- Winsock

			-- asset libs
			if not isModernVS() then
				linkoptions { "/DelayLoad:d3dx11_43.dll" }		-- D3D11
				linkoptions { "/DelayLoad:D3DX9_43.dll" }		-- D3D9
			end
			linkoptions { "/DelayLoad:FujiMiddleware.dll" }

			configuration { "not StaticLib", "windows", "x32 or native", "not Xbox360", "not PS3", "not Android" }
				linkoptions { "/DelayLoad:Assimp32.dll" }	-- Assimp

			configuration { "not StaticLib", "windows", "x64", "not Xbox360", "not PS3", "not Android" }
				linkoptions { "/DelayLoad:Assimp64.dll" }	-- Assimp
	end

-- Android --
configuration { "Android" }
	if not isVS() then
		-- vs-android defines these automatically
		defines { "ANDROID_NDK", "ANDROID", "__ANDROID__" }
	end
configuration { "not StaticLib", "Android" }
	links { "z", "log", "android", "GLESv1_CM", "jnigraphics" }
	links { "mad" }

-- XBox --
configuration { "Xbox" }
	defines { "WIN32", "_XBOX" }
	libdirs { "../lib/xbox" }

-- XBox 360 --
configuration { "Xbox360" }
	defines { "WIN32", "_XBOX" }
	libdirs { "../lib/x360" }

-- Playstation 3 --
configuration { "not StaticLib", "PS3" }
	libdirs { "../lib/ps3" }

-- Dreamcast __
configuration { "not StaticLib", "Dreamcast" }
	libdirs { "../lib/dc" }
	-- http://yam.20to4.net/dreamcast/hints/index.html <- some performance thoughts
--	compileoptions { "-m4-single", "-ml", "-mfused-madd", "-mfsca", "-mfsrra", "-mdalign" } -- "--ffast-math", "-mfmovd" ??

configuration { }
