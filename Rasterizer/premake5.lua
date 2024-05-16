workspace "Rasterizer"
	configurations { "Debug", "Release" }
	platforms { "Win32", "x64" }
	startproject "Rasterizer"

outputdir = "%{wks.location}/../Bin/"
objoutdir = "%{wks.location}/../Bin-Int/"
dependdir = "%{wks.location}/../Dependencies/"

project "Rasterizer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	files { "**.h", "**.inl", "**.cpp" }

	includedirs { "%{wks.location}", "include", dependdir .. "SDL2/include", dependdir .. "stb_image/include", dependdir .. "glm/include" }
	libdirs { dependdir .. "SDL2/lib/%{cfg.platform}" }

	links { "SDL2", "SDL2main" }

	targetdir (outputdir .. "%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}")
	objdir (objoutdir .. "%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}")
	characterset ("MBCS")
	debugdir "%{wks.location}../Bin/%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}"

	postbuildcommands {

		"xcopy /e /y /i /r \"%{wks.location}..\\Dependencies\\SDL2\\lib\\%{cfg.platform}\\*.lib\" \"%{wks.location}..\\Bin\\%{prj.name}\\%{cfg.buildcfg}\\%{cfg.platform}\\\"",
		"xcopy /e /y /i /r \"%{wks.location}..\\Dependencies\\SDL2\\dll\\%{cfg.platform}\\*.dll\" \"%{wks.location}..\\Bin\\%{prj.name}\\%{cfg.buildcfg}\\%{cfg.platform}\\\"",
		"xcopy /e /y /i /r \"%{wks.location}\\Resources\" \"%{wks.location}..\\Bin\\%{prj.name}\\%{cfg.buildcfg}\\%{cfg.platform}\\Resources\\\"",

		"EXIT /B 0"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"