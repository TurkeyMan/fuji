newoption {
	trigger = "dll",
	description = "Build Fuji as a shared library"
}

newoption {
	trigger = "sharedlib",
	description = "Build Fuji as a shared library"
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
		includePath = iif(installDir, installDir, "/usr/local") .. "/include"
		libPath = iif(installDir, installDir, "/usr/local") .. "/lib"

		-- copy includes --
		print("Copying includes to: " .. includePath)
		copyFiles("../../dist/include/Fuji", includePath .. "/Fuji", "**.h", "**.inl")
		copyFiles("../../dist/include/Haku", includePath .. "/Haku", "**.h", "**.inl")

		-- copy libs --
		print("Copying libs to: " .. libPath)
		copyFiles("../../dist/lib", libPath, "*.a", "*.so*")
--		copyFiles("../../dist/lib/i386-linux-gnu", libPath .. "/i386-linux-gnu", "*.a", "*.so*")
--		copyFiles("../../dist/lib/x86_64-linux-gnu", libPath .. "/x86_64-linux-gnu", "*.a", "*.so*")

		-- install documentation --
		print("Installing documentation: ** NOT DONE **")
	end
}

slnName = "Fuji"
if _OPTIONS.dll or _OPTIONS.sharedlib then
	slnName = "FujiDLL"
	fujiDll = true
end

solution (slnName)
	configurations { "Debug", "DebugOpt", "Release", "Retail" }
	platforms { "Native", "x32", "x64" }

	dofile "fujiproj.lua"
