project "AmberEditor"
    language "C++"
    cppdialect "C++20"
    
    files { "**.h", "**.inl", "**.cpp" }
    
    includedirs { 
        "%{wks.location}", 
        "include", 
        dependdir .. "SDL2/include", 
        dependdir .. "stb_image/include", 
        dependdir .. "glm/include", 
        "%{wks.location}/AmberGL/include" 
    }
    
    libdirs { 
        dependdir .. "SDL2/lib/%{cfg.platform}",
        builddir .. "AmberGL/lib/%{cfg.platform}/%{cfg.buildcfg}"
    }
    
    links { "SDL2", "SDL2main", "AmberGL" }
    
    targetdir (outputdir .. "%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}")
    objdir (objoutdir .. "%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}")
    debugdir "%{builddir}%{cfg.platform}/%{cfg.buildcfg}"
    
    postbuildcommands {
        "xcopy /Y \"%{dependdir}SDL2\\lib\\%{cfg.platform}\\*.lib\" \"%{cfg.targetdir}\\\"",
        "xcopy /Y \"%{dependdir}SDL2\\dll\\%{cfg.platform}\\*.dll\" \"%{cfg.targetdir}\\\"",
        
        "xcopy /Y \"%{builddir}AmberGL\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\*.dll\" \"%{cfg.targetdir}\\\"",
        
        "if not exist \"%{builddir}%{cfg.platform}\\%{cfg.buildcfg}\" mkdir \"%{builddir}%{cfg.platform}\\%{cfg.buildcfg}\"",
        
        "xcopy /Y /I /Q /D \"%{cfg.targetdir}\\*.exe\" \"%{builddir}%{cfg.platform}\\%{cfg.buildcfg}\\\"",
        "xcopy /Y /I /Q /D \"%{cfg.targetdir}\\*.dll\" \"%{builddir}%{cfg.platform}\\%{cfg.buildcfg}\\\"",
        
        "xcopy \"%{resdir}Models\\*\" \"%{builddir}%{cfg.platform}\\%{cfg.buildcfg}\\Resources\\Models\\\" /y /i /r /e /q",
        
        "EXIT /B 0"
    }
    
    buildoptions {"/sdl"}
    
    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"
        kind "ConsoleApp"
        runtime "Debug"
    
    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"
        kind "WindowedApp"
        runtime "Release"
        
    filter { "system:windows" }
        -- forces post-build commands to trigger even if nothing changed
        fastuptodate "Off"