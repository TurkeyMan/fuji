-- some code to take advantage of visual studios project macros

local function isVS()
	return _ACTION == "vs2010" or _ACTION == "vs2008" or _ACTION == "vs2005" or _ACTION == "vs2003" or _ACTION == "vs2002"
end

local function getConfigName(configName)
	-- how annoying that vs2010 decided to change the name of this macro! >_<
	if _ACTION == "vs2010" then
		return "$(Configuration)"
	elseif _ACTION == "vs2008" or _ACTION == "vs2005" or _ACTION == "vs2003" or _ACTION == "vs2002" then
		return "$(ConfigurationName)"
	end
	return configName
end

local configNames = {}
configNames.Debug = getConfigName("Debug")
configNames.DebugOpt = getConfigName("DebugOpt")
configNames.Release = getConfigName("Release")
configNames.Retail = getConfigName("Retail")

local platformNames = {}
platformNames.Android = iif(isVS(), "$(Platform)", "Android")
platformNames.iOS = iif(isVS(), "$(Platform)", "iOS")
platformNames.PS2 = iif(isVS(), "$(Platform)", "PS2")
platformNames.PS3 = iif(isVS(), "$(Platform)", "PS3")
platformNames.PSP = iif(isVS(), "$(Platform)", "PSP")
platformNames.Win32 = iif(isVS(), "$(Platform)", "Win32")
platformNames.Win64 = iif(isVS(), "$(Platform)", "Win64")
platformNames.XBox = iif(isVS(), "$(Platform)", "XBox")
platformNames.XBox360 = iif(isVS(), "$(Platform)", "XBox360")

-- configurations --

includedirs { "../Include/", "../../Private/Middleware/" }
libdirs { "../Lib/" }

configuration "Debug"
	defines { "DEBUG", "_DEBUG" }
	flags { "Symbols" }
	targetsuffix ("_" .. configNames.Debug)

configuration "DebugOpt"
	defines { "DEBUG", "_DEBUG" }
	flags { "Symbols", "Optimize" }
	targetsuffix ("_" .. configNames.DebugOpt)

configuration "Release"
	defines { "NDEBUG", "_RELEASE" }
	flags { "OptimizeSpeed" }
	targetsuffix ("_" .. configNames.Release)

configuration "Retail"
	defines { "NDEBUG", "_RETAIL" }
	flags { "OptimizeSpeed" }
	targetsuffix ("_" .. configNames.Retail)


-- platform specific config --

-- Linux --
configuration { "linux" }
	defines { "_LINUX" }
	links { "c", "m", "stdc++", "pthread", "GL", "GLU", "Xxf86vm", "X11", "ogg", "vorbis", "vorbisfile", "asound", "portaudio" }
	links { "z", "png", "mad", "json" }

-- OSX --
configuration { "macosx" }
	defines { "_OSX" }
	links { "c", "m", "stdc++", "pthread", "GL", "GLU", "Xxf86vm", "X11", "ogg", "vorbis", "vorbisfile" }
	links { "z", "png", "mad", "json" }

-- Windows --
configuration { "windows", "not Xbox360", "not PS3", "not Android" }
	defines { "WIN32", "_WINDOWS" }
	links { "ogg_static", "vorbis_static", "vorbisfile_static", "mad", "json" }
	linkoptions { "/Delay:unload" }

	linkoptions { "/DelayLoad:d3d11.dll", "/DelayLoad:d3dx11_42.dll" }		-- D3D11
	linkoptions { "/DelayLoad:d3d9.dll", "/DelayLoad:D3DX9_42.dll" }		-- D3D9
	linkoptions { "/DelayLoad:opengl32.dll" }								-- OpenGL
	linkoptions { "/DelayLoad:dsound.dll" }									-- Sound
	linkoptions { "/DelayLoad:xinput1_3.dll", "/DelayLoad:dinput8.dll" }	-- Input
	linkoptions { "/DelayLoad:ws2_32.dll" }									-- Winsock

	-- Windows 32 --
	configuration { "windows", "x32 or native", "not Xbox360", "not PS3" }
		libdirs { "../Lib/Middleware/vorbis/x86", "../Lib/Middleware/lglcd/x86" }

	-- Windows 64 --
	configuration { "windows", "x64", "not Xbox360", "not PS3" }
		libdirs { "../Lib/Middleware/vorbis/x64", "../Lib/Middleware/lglcd/x64" }

-- Android --
configuration "Android"
	if not isVS() then
		-- vs-android defines these automatically
		defines { "ANDROID_NDK", "ANDROID", "__ANDROID__" }
	end
	links { "z", "log", "android", "GLESv1_CM", "jnigraphics" }
	links { "mad", "json" }

-- XBox --
configuration "Xbox"
	defines { "WIN32", "_XBOX" }
	libdirs { "../Lib/Middleware/vorbis/xbox" }

-- XBox 360 --
configuration "Xbox360"
	defines { "WIN32", "_XBOX" }

-- Playstation 3 --
configuration "PS3"
	defines { "_PS3" }
	links { "mad", "json" }

configuration { }
