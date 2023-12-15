group "extern"
project "lyra.extern"
    kind "StaticLib"
    language "C++"
    targetdir "%{BASE_DIR}bin/%{cfg.buildcfg}"

    -- External libraries

    includeImGui()
    compileImGui()
    -- End external libraries
    configureFlags()

    setConfigurations()

    filter {}