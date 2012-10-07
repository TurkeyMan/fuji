newoption {
	trigger = "dll",
	description = "Build Haku as a shared library"
}

newoption {
	trigger = "sharedlib",
	description = "Build Haku as a shared library"
}

slnName = "Fuji"
if _OPTIONS.dll or _OPTIONS.sharedlib then
	slnName = "FujiDLL"
	fujiDll = true
end

solution "Haku"
	configurations { "Debug", "DebugOpt", "Release", "Retail" }
	platforms { "Native", "x32", "x64" }

	dofile "../../Fuji/Project/fujiproj.lua"
	dofile "hakuproj.lua"
