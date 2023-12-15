group "editor"

project "lyra.editor"
    kind "StaticLib"
    language "C++"
    targetdir "%{BASE_DIR}bin/%{cfg.buildcfg}"

    files { 
        "%{BASE_DIR}packages/LyraEditor/**.h", 
        "%{BASE_DIR}packages/LyraEditor/**.hpp", 
        "%{BASE_DIR}packages/LyraEditor/**.inl", 
        "%{BASE_DIR}packages/LyraEditor/**.cpp", 
        "%{BASE_DIR}packages/LyraEditor/**.c" 
    }

    includedirs {
        "%{BASE_DIR}packages/Lyra",
        "%{BASE_DIR}packages/LyraEditor"
    }

    configureFlags()

    -- External libraries
    -- End external libraries

    setConfigurations()

    filter {}