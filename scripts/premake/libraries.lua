function includeImGui()
    filter {}
        defines { "IMGUI_DEFINE_MATH_OPERATORS","IMGUI_DISABLE_OBSOLETE_KEYIO" }
        includedirs "%{BASE_DIR}external/imgui"
end

function compileImGui()
    filter {}
    files {
        "%{BASE_DIR}external/imgui/*.cpp",
        "%{BASE_DIR}external/imgui/*.h"
    }
    
end

function includeCore()
end

function linkCore()
end