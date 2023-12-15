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
        "%{BASE_DIR}packages/Lyra/include",
        "%{BASE_DIR}packages/LyraEditor/include",
        "%{BASE_DIR}packages/LyraApplication/include"
    }

    filter { "system:windows" }
        fastuptodate ("Off")
        postbuildcommands { "robocopy ../packages/Lyra/shaders %{cfg.buildtarget.directory}shaders /s /NJS /NJH /NDL /NP /NC /NS" }
        
    configureFlags()

    -- External libraries
    includeALL()
    includeImGui()
    linkALL()
    -- End external libraries

    setConfigurations()

    filter {}
