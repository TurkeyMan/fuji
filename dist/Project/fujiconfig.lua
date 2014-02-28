-- some code to take advantage of visual studios project macros

function isVS()
	return _ACTION == "vs2013" or _ACTION == "vs2012" or _ACTION == "vs2010" or _ACTION == "vs2008" or _ACTION == "vs2005" or _ACTION == "vs2003" or _ACTION == "vs2002"
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

includedirs { "../include/", "../include/d2/" }

if os.get() == "windows" then
	separator = "_"
else
	separator = "-"
end

configuration "Debug"
	defines { "DEBUG", "_DEBUG" }
	flags { "Symbols" }
	optimize "Debug"
	targetsuffix (separator .. configNames.Debug)

configuration "DebugOpt"
	defines { "DEBUG", "_DEBUG" }
	flags { "Symbols" }
	optimize "On"
	targetsuffix (separator .. configNames.DebugOpt)

configuration "Release"
	defines { "NDEBUG", "_RELEASE" }
	optimize "Full"
	targetsuffix (separator .. configNames.Release)

configuration "Retail"
	defines { "NDEBUG", "_RETAIL" }
	optimize "Full"
	targetsuffix (separator .. configNames.Retail)


-- platform specific config --

-- Linux --
configuration { "linux" }
	links { "c", "m", "stdc++", "pthread", "GL", "GLU", "Xxf86vm", "X11", "ogg", "vorbis", "vorbisfile" }
	links { "z", "mad" }
--	links { "asound" }
--	links { "portaudio" }
--	links { "pulse" }
	links { "openal" }

-- OSX --
configuration { "macosx" }
	links { "c", "m", "stdc++", "pthread", "GL", "GLU", "Xxf86vm", "X11", "ogg", "vorbis", "vorbisfile" }
	links { "z", "png", "mad" }

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
			linkoptions { "/DelayLoad:d3d11.dll" }									-- D3D11
			linkoptions { "/DelayLoad:d3d9.dll" }									-- D3D9
			linkoptions { "/DelayLoad:opengl32.dll" }								-- OpenGL
			linkoptions { "/DelayLoad:dsound.dll" }									-- Sound
			linkoptions { "/DelayLoad:xinput1_3.dll", "/DelayLoad:dinput8.dll" }	-- Input
			linkoptions { "/DelayLoad:ws2_32.dll" }									-- Winsock
	end

-- Android --
configuration "Android"
	if not isVS() then
		-- vs-android defines these automatically
		defines { "ANDROID_NDK", "ANDROID", "__ANDROID__" }
	end
	links { "z", "log", "android", "GLESv1_CM", "jnigraphics" }
	links { "mad" }

-- XBox --
configuration "Xbox"
	defines { "WIN32", "_XBOX" }
	libdirs { "../lib/xbox" }

-- XBox 360 --
configuration "Xbox360"
	defines { "WIN32", "_XBOX" }
	libdirs { "../lib/x360" }

-- Playstation 3 --
configuration "PS3"
	libdirs { "../lib/ps3" }

-- Dreamcast __
configuration "Dreamcast"
	libdirs { "../lib/dc" }
	-- http://yam.20to4.net/dreamcast/hints/index.html <- some performance thoughts
--	compileoptions { "-m4-single", "-ml", "-mfused-madd", "-mfsca", "-mfsrra", "-mdalign" } -- "--ffast-math", "-mfmovd" ??


configuration { }
