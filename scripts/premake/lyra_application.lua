group "20 app"

project "lyra.app"
    kind "ConsoleApp"
    links { "lyra.core", "lyra.render" }
    language "C++"
    targetdir "%{BASE_DIR}bin/lyra.app/%{cfg.buildcfg}"
    
    files {
        "%{BASE_DIR}packages/LyraApp/**.h",
        "%{BASE_DIR}packages/LyraApp/**.hpp",
        "%{BASE_DIR}packages/LyraApp/**.inl",
        "%{BASE_DIR}packages/LyraApp/**.cpp",
        "%{BASE_DIR}packages/LyraApp/**.c"
    }
    
    includedirs {
        "%{BASE_DIR}packages/Lyra",
        "%{BASE_DIR}packages/LyraRender",
        "%{BASE_DIR}packages/LyraApp"
    }
    
    filter { "system:windows" }
        fastuptodate ("Off")
        postbuildcommands { "robocopy ../packages/LyraRender/shaders/GLSL %{cfg.buildtarget.directory}shaders /s /NJS /NJH /NDL /NP /NC /NS" }

    configureFlags()
    
    -- External libraries
    includeTaskFlow()
    includeGLFW()
    includeVulkan()

    linkGLFW()
    linkVulkan()

    -- End external libraries
    
    setConfigurations()
    
    filter {}

project "lyra.app.editor"
    kind "WindowedApp"
    links { "lyra.core" , "lyra.render", "lyra.editor" }
    language "C++"
    targetdir "%{BASE_DIR}bin/lyra.app.editor./%{cfg.buildcfg}"

    files { 
        "%{BASE_DIR}packages/LyraAppEditor/**.h", 
        "%{BASE_DIR}packages/LyraAppEditor/**.hpp", 
        "%{BASE_DIR}packages/LyraAppEditor/**.inl", 
        "%{BASE_DIR}packages/LyraAppEditor/**.cpp", 
        "%{BASE_DIR}packages/LyraAppEditor/**.c" 
    }

    includedirs {
        "%{BASE_DIR}packages/Lyra",
        "%{BASE_DIR}packages/LyraRender",
        "%{BASE_DIR}packages/LyraEditor",
        "%{BASE_DIR}packages/LyraAppEditor"
    }

    filter { "system:windows" }
        fastuptodate ("Off")
        postbuildcommands { "robocopy ../packages/LyraRender/shaders/GLSL %{cfg.buildtarget.directory}shaders /s /NJS /NJH /NDL /NP /NC /NS" }
        
    configureFlags()
    -- External libraries
    links { "lyra.external.imgui" }
    includeImGui()
    includeTaskFlow()
    includeGLFW()
    includeVulkan()

    linkGLFW()
    linkVulkan()

    setupwxWidgets()

 -- End external libraries

    setConfigurations()

    filter {}
