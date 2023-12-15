group "app"

project "lyra.app.editor"
    kind "ConsoleApp"
    links { "lyra.core" ,"lyra.editor" }

    language "C++"
    targetdir "%{BASE_DIR}bin/lyraApplication/%{cfg.buildcfg}"

    files { 
        "%{BASE_DIR}packages/LyraApplication/**.h", 
        "%{BASE_DIR}packages/LyraApplication/**.hpp", 
        "%{BASE_DIR}packages/LyraApplication/**.inl", 
        "%{BASE_DIR}packages/LyraApplication/**.cpp", 
        "%{BASE_DIR}packages/LyraApplication/**.c" 
    }

    includedirs {
        "%{BASE_DIR}packages/Lyra",
        "%{BASE_DIR}packages/LyraEditor",
        "%{BASE_DIR}packages/LyraApplication"
    }

    filter { "system:windows" }
        fastuptodate ("Off")
        
    configureFlags()

    -- External libraries
    -- End external libraries

    setConfigurations()

    filter {}
