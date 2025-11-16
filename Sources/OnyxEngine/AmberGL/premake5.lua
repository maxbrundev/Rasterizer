project "AmberGL"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	defines { "AMBERGL_EXPORT" }
	files {"**.h", "**.inl", "**.cpp"}
	includedirs {".", "include", dependdir .. "glm/include"}

	targetdir(outputdir .. "%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}")
	objdir(objoutdir .. "%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}")
	debugdir "%{wks.location}/../../Build/%{prj.name}/%{cfg.platform}/%{cfg.buildcfg}"

	characterset("MBCS")

	postbuildcommands {
		"xcopy /E /R /I /Y \"%{prj.location}include\\%{prj.name}\" \"%{wks.location}..\\..\\Build\\%{prj.name}\\include\\%{prj.name}\"",
		"xcopy /E /R /I /Y \"%{prj.location}*.h\" \"%{wks.location}..\\..\\Build\\%{prj.name}\\include\\%{prj.name}\"",

		"xcopy /Y /I \"%{cfg.targetdir}\\*.lib\" \"%{wks.location}..\\..\\Build\\%{prj.name}\\lib\\%{cfg.platform}\\%{cfg.buildcfg}\"",
		"xcopy /Y /I \"%{cfg.targetdir}\\*.dll\" \"%{wks.location}..\\..\\Build\\%{prj.name}\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\"",

		"xcopy /D /I /Y /Q \"%{wks.location}..\\..\\Bin\\%{cfg.platform}\\Release\\%{prj.name}\\*.dll\" \"%{wks.location}..\\..\\Build\\%{cfg.platform}\\%{cfg.buildcfg}\"",
		"xcopy /D /I /Y /Q \"%{wks.location}..\\..\\Bin\\%{cfg.platform}\\Release\\%{prj.name}\\*.exe\" \"%{wks.location}..\\..\\Build\\%{cfg.platform}\\%{cfg.buildcfg}\"",
		"xcopy /D /I /Y /Q \"%{wks.location}..\\..\\Bin\\%{cfg.platform}\\Debug\\%{prj.name}\\*.dll\" \"%{wks.location}..\\..\\Build\\%{cfg.platform}\\%{cfg.buildcfg}\"",
		"xcopy /D /I /Y /Q \"%{wks.location}..\\..\\Bin\\%{cfg.platform}\\Debug\\%{prj.name}\\*.exe\" \"%{wks.location}..\\..\\Build\\%{cfg.platform}\\%{cfg.buildcfg}\"",

		"EXIT /B 0"
	}

	buildoptions {"/sdl"}

	filter "configurations:Debug"
	defines {"DEBUG"}
	symbols "On"
	runtime "Debug"

	filter "configurations:Release"
	defines {"NDEBUG"}
	optimize "On"
	runtime "Release"
