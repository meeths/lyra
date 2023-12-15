group "tests"

project "lyra.tests"
    kind "ConsoleApp"
    links { "lyra.core" }

    language "C++"
    targetdir "%{BASE_DIR}bin/lyraTests/%{cfg.buildcfg}"

    files{
        "%{BASE_DIR}packages/LyraTests/**.cpp",
        "%{BASE_DIR}external/googletest/googletest/src/gtest-all.cc",
        "%{BASE_DIR}external/googletest/googletest/src/gtest_main.cc",
        "%{BASE_DIR}external/googletest/googlemock/src/gmock-all.cc",
    }

   includedirs { 
    "%{BASE_DIR}packages/Lyra",
    "%{BASE_DIR}external/googletest/googletest/include",
    "%{BASE_DIR}external/googletest/googlemock/include",
    "%{BASE_DIR}external/googletest/googletest",
    "%{BASE_DIR}external/googletest/googlemock"
   }

    -- External libraries
    includeCore()
    -- End external libraries

    configureFlags()
    setConfigurations()

    filter {}
