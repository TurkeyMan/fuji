newoption {
	trigger = "dll",
	description = "Build Fuji as a shared library"
}

slnName = "Fuji"
if _OPTIONS.dll then
	slnName = "FujiDLL"
	fujiDll = true
end

solution (slnName)
	configurations { "Debug", "DebugOpt", "Release", "Retail" }
	platforms { "Native", "x32", "x64" }

	dofile "fujiproj.lua"
