newoption {
	trigger = "install-dir",
	value = "PATH",
	description = "Target directory for installation (Default: /usr/local)"
}

newaction {
	trigger = "install",
	description = "Install the software",
	execute = function ()
		local function copyFile(srcFile, destPath)
			local destFile = destPath .. '/' .. path.getname(srcFile)
			os.mkdir(destPath)
			os.copyfile(srcFile, destFile)

			if os.is("linux") then
				-- hook up version-less symbolic links for libs
				local extPos = string.findlast(destFile, ".so")
				if extPos then
					local ext = destFile:sub(extPos)
					if ext:len() > 5 then
--						print("Creating links to shared libs...")
						print("  " .. destFile:sub(1,extPos + 2 + 2))
						os.execute("ln -snf " .. destFile .. " " .. destFile:sub(1,extPos + 2 + 2))
						print("  " .. destFile:sub(1,extPos + 2))
						os.execute("ln -snf " .. destFile .. " " .. destFile:sub(1,extPos + 2))
					end
				end
			end
		end

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
				copyFile(f, path.getdirectory(dest))
			end
		end

		local installDir = _OPTIONS["install-dir"]
		includePath = iif(installDir, installDir, "/usr/local") .. "/include"
		libPath = iif(installDir, installDir, "/usr/local") .. "/lib"
		docPath = iif(installDir, installDir, "/usr/local") .. "/share/doc"

		-- copy includes --
		print("Copying includes to: " .. includePath)
		copyFiles("dist/include/Fuji", includePath .. "/Fuji", "**.h", "**.inl")
		copyFiles("dist/include/Haku", includePath .. "/Haku", "**.h", "**.inl")
		copyFiles("dist/include/d2", includePath .. "/d2", "**.d", "**.di")

		-- copy libs --
		print("Copying libs to: " .. libPath)
		if os.is64bit() then
			copyFiles("dist/lib/linux-x86_64", libPath, "libFuji*", "libHaku*")
		else
			copyFiles("dist/lib/linux-i386", libPath, "libFuji*", "libHaku*")
		end

		-- install documentation --
		print("Installing documentation to: " .. docPath)
		copyFiles("dist/doc", docPath .. "/Fuji", "**.chm")
	end
}

solution "Fuji"
	if _ACTION == "gmake" then
		configurations { "Release", "Debug", "DebugOpt", "Retail" }
	else
		configurations { "Debug", "DebugOpt", "Release", "Retail" }
	end

	if os.get() == "windows" then
		platforms { "x32", "x64" }
	end

	-- Static lib project
	dofile "Fuji/Project/fujiproj.lua"

	-- Shared lib project
	fujiDll = true
	dofile "Fuji/Project/fujiproj.lua"

	-- Haku project
	dofile "Haku/Project/hakuproj.lua"

	-- D bindings
	dofile "Fuji/Project/fujidproj.lua"

	-- Asset compiler
	if os.is("windows") then
		dofile "Fuji/Project/fujiasset.lua"
	end
