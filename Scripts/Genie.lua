solution "PantherEngine"
	location ".."
	
	configurations {
		"Debug",
		"Release",
	}
	
	platforms {
		"x64"
	}
		
	configuration {}
		defines { "_UNICODE", "UNICODE" }
		flags { "Unicode", "WinMain" }
	
	configuration "Debug"
		defines { "CONFIG_DEBUG" }
		flags { "Symbols" }
		
	configuration "Release"
		defines { "CONFIG_RELEASE" }
		flags { "Optimize" }
	
	language "C++"

PROJ_DIR = path.getabsolute("..")

if _ACTION == "vs2012" or _ACTION == "vs2013" 
or _ACTION == "vs2015" or _ACTION == "vs2017" then
	local action = premake.action.current()
	action.vstudio.windowsTargetPlatformVersion = "10.0.15063.0"
end

project "Engine"
	kind "WindowedApp"
	
	location "../Engine/Intermediate"

	debugdir "../Engine/Binaries"
	objdir "../Engine/Intermediate"
	targetdir "../Engine/Binaries"
	
	includedirs {
		"../Engine/Include",
	}
	
	files {
		"../Engine/Source/*.cpp",
		"../Engine/Source/*.hpp",
		"../Engine/Source/*.h", -- I prefer .hpp over .h, kept for legacy. Remove when possible.
		"../Engine/Source/Resources/*.rc",
		"../Engine/Source/Resources/*.h",
	}
		
	libdirs {
		"../Engine/Libraries/",
	}
	
	links { 
		"D3D12", 
		"DXGI", 
		"D3DCompiler",
		"assimp-vc140-mt",
	}
	
	configuration {}
		postbuildcommands { "copy ..\\..\\assimp-vc140-mt.dll ..\\Binaries\\assimp-vc140-mt.dll" }
	