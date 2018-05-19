solution "Samples"
	configurations { "Debug", "DebugOpt", "Release", "Retail" }
--	platforms { "Native", "x32", "x64", "Xbox360", "PS3" }
--	platforms { "Native", "Xbox" }
	platforms { "Native", "x32", "x64" }

	-- include the fuji project...
	dofile "../../Fuji/Project/fujiproj.lua"

	-- include each of the samples --
	include "3DBox"
--	include "AudioCapture"
	include "Compute"
	include "HelloWorld"
	include "InputTest"
	include "ModelTest"
	include "ModelViewer"
	include "RenderTarget"

	-- include the Haku project...
	dofile "../../Haku/Project/hakuproj.lua"

	-- include each of the samples --
	include "UI"

