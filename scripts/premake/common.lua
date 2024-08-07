BASE_DIR = _MAIN_SCRIPT_DIR .. "/"

function configureFlags()
    flags { "FatalCompileWarnings", "MultiProcessorCompile" }    
    --staticruntime "Off"
    filter {"configurations:Release"}
        flags { "LinkTimeOptimization" }        
    filter {}
    --defines { "NOMINMAX" }
    toolset "clang"
    cppdialect "C++20"
    editAndContinue "Off"
    exceptionhandling "Off"
    characterset "Unicode"
end

function setConfigurations()
    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"
        runtime "Release"
end

workspace "lyra"
    location "%{BASE_DIR}projects"
    configurations { "Debug", "Release" }
    platforms { "Win64" }

    targetdir "%{BASE_DIR}bin/lyra/%{cfg.buildcfg}"

    filter { "platforms:Win64" }
    system "Windows"
        architecture "x64"

    filter {}