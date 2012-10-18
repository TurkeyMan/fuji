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
				dest = destPath .. "/" .. path.getrelative(srcPath, f)
				print("  " .. dest)
				os.mkdir(path.getdirectory(dest))
				os.copyfile(f, dest)
			end
		end

		local installDir = _OPTIONS["install-dir"]
		includePath = iif(installDir, installDir, "/usr/local") .. "/include"
		libPath = iif(installDir, installDir, "/usr/local") .. "/lib"

		-- copy includes --
		print("Copying includes to: " .. includePath)
		copyFiles("dist/include/Fuji", includePath .. "/Fuji", "**.h", "**.inl")
		copyFiles("dist/include/Haku", includePath .. "/Haku", "**.h", "**.inl")

		-- copy libs --
		print("Copying libs to: " .. libPath)
		copyFiles("dist/lib", libPath, "*.a", "*.so*")
--		copyFiles("dist/lib/i386-linux-gnu", libPath .. "/i386-linux-gnu", "*.a", "*.so*")
--		copyFiles("dist/lib/x86_64-linux-gnu", libPath .. "/x86_64-linux-gnu", "*.a", "*.so*")

		if os.get() == "linux" then
			-- hook up version-less symbolic links for libs
			files = os.matchfiles(libPath .. "/*.so*")
			print("Creating links to shared libs...")
			for _, f in ipairs(files) do
				local extPos = string.findlast(f, ".so")
				local ext = f:sub(extPos)
				if ext:len() > 5 then
					print("  " .. f:sub(1,extPos + 2 + 2))
					os.execute("ln -snf " .. f .. " " .. f:sub(1,extPos + 2 + 2))
					print("  " .. f:sub(1,extPos + 2))
					os.execute("ln -snf " .. f .. " " .. f:sub(1,extPos + 2))
				end
			end
		end

		-- install documentation --
		print("Installing documentation: ** NOT DONE **")
	end
}

solution "Fuji"
	if _ACTION == "gmake" then
		configurations { "Release", "Debug", "DebugOpt", "Retail" }
	else
		configurations { "Debug", "DebugOpt", "Release", "Retail" }
	end

	if os.get() == "windows" then
		platforms { "Native", "x32", "x64" }
	else
		platforms { "Native" }
	end

	-- Static lib project
	dofile "Fuji/Project/fujiproj.lua"

	-- Shared lib project
	fujiDll = true
	dofile "Fuji/Project/fujiproj.lua"

	-- Haku project
	dofile "Haku/Project/hakuproj.lua"

