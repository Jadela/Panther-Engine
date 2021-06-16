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
	
	configuration "x64"
		defines { "PLATFORM_WINDOWS" }
	
	language "C++"

if string.match(_ACTION, "vs") ~= nil then
	local action = premake.action.current()
	action.vstudio.windowsTargetPlatformVersion = "10.0.16299.0"
	action.vstudio.intDirAbsolute = false
end

project "Engine"
	kind "WindowedApp"
	
	location "../Engine/Intermediate/ProjectFiles"

	debugdir "../Engine/Binaries"
	objdir "../Engine/Intermediate"
	targetdir "../Engine/Binaries"
	
	includedirs {
		"../Engine/Include",
		"../Engine/Source/Core/Public"
	}
	
	files {
		"../Engine/Source/*.cpp",
		"../Engine/Source/*.hpp",
		"../Engine/Source/*.h", -- I prefer .hpp over .h, kept for legacy. Remove when possible.
		"../Engine/Source/Resources/*.rc",
		"../Engine/Source/Resources/*.h",
	}
		
	libdirs {
		"../Engine/Libraries",
	}
	
	links { 
		--"Core",
		"D3D12", 
		"DXGI", 
		"D3DCompiler",
		"assimp-vc140-mt",
	}
	
	configuration {}
		postbuildcommands { "copy ..\\..\\..\\assimp-vc140-mt.dll ..\\..\\Binaries\\assimp-vc140-mt.dll" }
	
project "Core"
	kind "StaticLib"
	
	location "../Engine/Intermediate/ProjectFiles"
	
	objdir "../Engine/Intermediate/Core"
	targetdir "../Engine/Libraries"
	
	includedirs {}
	
	files {
		"../Engine/Source/Core/Public/*.hpp",
		"../Engine/Source/Core/Private/*.cpp",
		"../Engine/Source/Core/Private/*.hpp"
	}
