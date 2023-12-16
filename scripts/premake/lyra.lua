group "00 engine"

project "lyra.core"
    kind "StaticLib"
    language "C++"
    targetdir "%{BASE_DIR}bin/%{cfg.buildcfg}"

    files { 
        "%{BASE_DIR}packages/Lyra/**.h", 
        "%{BASE_DIR}packages/Lyra/**.hpp", 
        "%{BASE_DIR}packages/Lyra/**.inl", 
        "%{BASE_DIR}packages/Lyra/**.cpp", 
        "%{BASE_DIR}packages/Lyra/**.c" 
    }

    includedirs {
        "%{BASE_DIR}packages/Lyra"
    }

    configureFlags()

    -- External libraries
    includeTaskFlow()
    includeEntt()
    includeTracy()

    links { "lyra.external.tracy" }

    -- End external libraries

    setConfigurations()

    filter {}

project "lyra.render"
    kind "StaticLib"
    language "C++"
    targetdir "%{BASE_DIR}bin/%{cfg.buildcfg}"
    
    files {
        "%{BASE_DIR}packages/LyraRender/**.h",
        "%{BASE_DIR}packages/LyraRender/**.hpp",
        "%{BASE_DIR}packages/LyraRender/**.inl",
        "%{BASE_DIR}packages/LyraRender/**.cpp",
        "%{BASE_DIR}packages/LyraRender/**.c"
    }
    
    includedirs {
        "%{BASE_DIR}packages/Lyra",
        "%{BASE_DIR}packages/LyraRender"
    }

    links { "lyra.core" }

    configureFlags()
    
    -- External libraries
    includeTaskFlow()
    includeEntt()
    includeTracy()
    includeGLFW()
    includeVulkan()

    links { "lyra.external.tracy" }
    -- End external libraries
    
    setConfigurations()
    
    filter {}