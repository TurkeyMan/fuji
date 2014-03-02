--
-- Set the output directory for lots of configurations
--

configuration { }
	targetdir("../../dist/lib")

configuration { "windows", "x32 or native", "not Xbox360", "not PS3", "not Android" }
	targetdir("../../dist/lib/x86")
configuration { "windows", "x64", "not Xbox360", "not PS3", "not Android" }
	targetdir("../../dist/lib/x64")
configuration { "linux" } -- TODO: support cross-compiling properly...
	if os.is64bit() then
		targetdir("../../dist/lib/linux-x86_64")
	else
		targetdir("../../dist/lib/linux-i386")
	end
configuration { "android", "arm7" }
	targetdir("../../dist/lib/android-arm7")
configuration { "android", "mips" }
	targetdir("../../dist/lib/android-mips")
configuration { "android", "x86" }
	targetdir("../../dist/lib/android-x86")

configuration { }
