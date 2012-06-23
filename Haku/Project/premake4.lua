newoption {
	trigger = "dll",
	description = "Build Haku as a shared library"
}

newoption {
	trigger = "sharedlib",
	description = "Build Haku as a shared library"
}

newoption {
	trigger = "install-dir",
	value = "PATH",
	description = "Target directory for installation (Default: /usr/local)"
}

newaction {
	trigger = "install",
	description = "Install the software",
	execute = function ()
		local function copyFiles(srcPath, destPath, ...)
			-- append srcPath to the patterns --
			local patterns = { }
			for _, f in ipairs(arg) do
				table.insert(patterns, srcPath .. "/" .. f)
			end

			-- gather and copy files --
			files = os.matchfiles(unpack(patterns))
			for _, f in ipairs(files) do
				dest = destPath .. path.getrelative(srcPath, f)
				print("  " .. dest)
				os.mkdir(path.getdirectory(dest))
				os.copyfile(f, dest)
			end
		end

		installDir = _OPTIONS["install-dir"]
		includePath = iif(installDir, installDir, "/usr/local") .. "/include/Haku/"
		libPath = iif(installDir, installDir, "/usr/local")

		-- copy includes --
		print("Copying includes to: " .. includePath)
		copyFiles("../Include", includePath, "**.h", "**.inl")

		-- copy libs --
		print("Copying libs to: " .. libPath .. "/lib")
		copyFiles("../Lib", libPath .. "/lib/", "*.a", "*.so")
--		copyFiles("../Lib/x32", libPath, "*.a", "*.so")
--		copyFiles("../Lib/x64", libPath, "*.a", "*.so")

		-- install documentation --
		print("Installing documentation: ** NOT DONE **")
	end
}

slnName = "Fuji"
if _OPTIONS.dll or _OPTIONS.sharedlib then
	slnName = "FujiDLL"
	fujiDll = true
end

solution "Haku"
	configurations { "Debug", "DebugOpt", "Release", "Retail" }
	platforms { "Native", "x32", "x64" }

	dofile "../../Fuji/Private/Project/fujiproj.lua"
	dofile "hakuproj.lua"
