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
    -- End external libraries

    setConfigurations()

    filter {}