-- configurations --

configuration "Debug"
	defines { "DEBUG", "_DEBUG" }
	flags { "Symbols" }
	targetsuffix "_Debug"

configuration "DebugOpt"
	defines { "DEBUG", "_DEBUG" }
	flags { "Symbols", "OptimizeSpeed" }
	targetsuffix "_DebugOpt"

configuration "Release"
	defines { "NDEBUG", "_RELEASE" }
	flags { "OptimizeSpeed" }
	targetsuffix "_Release"

configuration "Retail"
	defines { "NDEBUG", "_RETAIL" }
	flags { "OptimizeSpeed" }


-- platform specific config --

-- Linux --
configuration { "linux" }
	defines { "_LINUX" }
	links { "c", "m", "stdc++", "pthread", "GL", "GLU", "Xxf86vm", "X11", "ogg", "vorbis", "vorbisfile", "asound", "portaudio" }
	links { "z", "mad", "png", "json" }

-- OSX --
configuration { "macosx" }
	defines { "_OSX" }
	links { "c", "m", "stdc++", "pthread", "GL", "GLU", "Xxf86vm", "X11", "ogg", "vorbis", "vorbisfile" }
	links { "z", "mad", "png", "json" }

-- Windows --
configuration { "windows", "not Xbox360", "not PS3" }
	defines { "WIN32", "_WINDOWS" }
	links { "ogg_static", "vorbisfile_static", "mad", "json" }

	-- Windows 32 --
	configuration { "windows", "x32 or native", "not Xbox360", "not PS3" }
		libdirs { "../Lib/Middleware/vorbis/x86", "../Lib/Middleware/lglcd/x86" }
		
	-- Windows 64 --
	configuration { "windows", "x64", "not Xbox360", "not PS3" }
		libdirs { "../Lib/Middleware/vorbis/x64", "../Lib/Middleware/lglcd/x64" }

-- XBox --
configuration "Xbox"
	defines { "WIN32", "_XBOX" }
	libdirs { "../Lib/Middleware/vorbis/xbox" }
	links { "mad", "json" }

-- XBox 360 --
configuration "Xbox360"
	defines { "WIN32", "_XBOX" }
	links { "mad", "json" }

-- Playstation 3 --
configuration "PS3"
	defines { "_PS3" }
	links { "mad", "json" }
