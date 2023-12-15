BASE_DIR = "../../"

function configureFlags()
    flags { "FatalWarnings", "MultiProcessorCompile" }    
    staticruntime "Off"
    filter {"configurations:Release"}
        flags { "LinkTimeOptimization" }        
    filter {}
    defines { "NOMINMAX" }
    cppdialect "C++20"
    editAndContinue "Off"
end

function setConfigurations()
    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
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