include "scripts/premake/lyra_extern.lua"

group "core"

project "lyra.core"
    kind "StaticLib"
    language "C++"
    targetdir "%{BASE_DIR}bin/%{cfg.buildcfg}"
    links { "lyra.extern" }

    files { 
        "%{BASE_DIR}packages/Lyra/**.h", 
        "%{BASE_DIR}packages/Lyra/**.hpp", 
        "%{BASE_DIR}packages/Lyra/**.inl", 
        "%{BASE_DIR}packages/Lyra/**.cpp", 
        "%{BASE_DIR}packages/Lyra/**.c" 
    }

    includedirs {
        "%{BASE_DIR}packages/Lyra/include"
    }

    configureFlags()

    -- External libraries
    includeCore()
    -- End external libraries

    setConfigurations()

    filter {}