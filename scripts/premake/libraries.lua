function linkXInput()
    links "xinput.lib"
end

function includeVulkan()
    includedirs "%{BASE_DIR}external/vulkan_1.2.198.1/Include"

end

function linkVulkan()
    filter { "platforms:Win32" }
        libdirs "%{BASE_DIR}external/vulkan_1.2.198.1/Lib32"
    filter { "platforms:Win64"}
        libdirs "%{BASE_DIR}external/vulkan_1.2.198.1/Lib"
        postbuildcommands { "{copy} %{wks.location}../external/vulkan_1.2.198.1/Bin/shaderc_shared.dll %{cfg.buildtarget.directory}" }

    filter "kind:not StaticLib"
        links "vulkan-1.lib"
        links "shaderc_shared.lib"

    filter "configurations:Debug"
        ignoredefaultlibraries { "msvcrt.lib" } 

    filter "configurations:Release"
        ignoredefaultlibraries { "msvcrtd.lib" } 

    filter {}
    
    
end

function includeGenerator()
    includedirs "%{BASE_DIR}external/generator/include"

end

function linkGenerator()
    filter "configurations:Debug"
        libdirs "%{BASE_DIR}external/generator/lib/Debug"
    filter "configurations:Release"
        libdirs "%{BASE_DIR}external/generator/lib/Release"

    filter {}

    links "generator.lib"
end

function includeGLFW()
    filter { "platforms:Win64" }
        includedirs "%{BASE_DIR}external/glfw/glfw-3.3.8.bin.WIN64/include"
    filter {}
end

function linkGLFW()
    filter { "platforms:Win64", "action:vs2019" }
        libdirs "%{BASE_DIR}external/glfw/glfw-3.3.8.bin.WIN64/lib-vc2019"
    filter { "platforms:Win64", "action:vs2022" }
        libdirs "%{BASE_DIR}external/glfw/glfw-3.3.8.bin.WIN64/lib-vc2022"

    links "glfw3"

    filter {}

end


function includeTracy()
    filter {}
    includedirs "%{BASE_DIR}external/tracy/public"
    defines { "TRACY_ENABLE", "TRACY_ON_DEMAND" }
end

function includeFMOD()
    filter {}
        includedirs "%{BASE_DIR}external/fmod/api/core/inc"
end

function linkFMOD()
    filter { "platforms:Win64"}
        libdirs "%{BASE_DIR}external/fmod/api/core/lib/x64"
	
	filter "configurations:Release"
        postbuildcommands { "{copy} %{wks.location}../external/FMOD/api/core/lib/x64/fmod.dll %{cfg.buildtarget.directory}" }
		links "fmod_vc.lib"

    filter "configurations:Debug"
        postbuildcommands { "{copy} %{wks.location}../external/FMOD/api/core/lib/x64/fmodL.dll %{cfg.buildtarget.directory}" }
		links "fmodL_vc.lib"

    filter {}
end
    
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

function includeSTB()
    includedirs "%{BASE_DIR}external/stb"
end    

function includeAssimp()
    filter { "platforms:Win64" }
        includedirs "%{BASE_DIR}external/assimp-5.0.1/include"
    filter {}
end

function linkAssimp()
    filter "configurations:Debug"
        libdirs "%{BASE_DIR}external/assimp-5.0.1/lib/x64/debug"
        links "assimp-vc142-mtd.lib"
        postbuildcommands { "{copy} %{wks.location}../external/assimp-5.0.1/lib/x64/debug/assimp-vc142-mtd.dll %{cfg.buildtarget.directory}" }

    filter "configurations:Release"
        libdirs "%{BASE_DIR}external/assimp-5.0.1/lib/x64/release"
        links "assimp-vc142-mt.lib"
        postbuildcommands { "{copy} %{wks.location}../external/assimp-5.0.1/lib/x64/release/assimp-vc142-mt.dll %{cfg.buildtarget.directory}" }

    filter {}
end

function includeLua()
    filter {}
    includedirs "%{BASE_DIR}external/lua-5.4.0_win64/include"
end

function linkLua()
    filter { "platforms:Win64"}
        libdirs "%{BASE_DIR}external/lua-5.4.0_win64"
        linkoptions { '/NODEFAULTLIB:"libcmt.lib"' }
    
    filter "kind:not StaticLib"
        links "lua54.lib"
end

function includeSOL2()
    filter {}
        includedirs "%{BASE_DIR}external/sol2/include"
end


function includeEnTT()
    filter {}
    includedirs "%{BASE_DIR}external/entt/single_include"
end

function includeCereal()
    filter {}
    includedirs "%{BASE_DIR}external/cereal/include"
end

function includeJq()
    filter {}
    includedirs "%{BASE_DIR}external/jq"
end

function compileJq()
    filter {}
    files {
        "%{BASE_DIR}external/jq/*.cpp",
        "%{BASE_DIR}external/jq/*.h"
    }
    filter {"platforms:Win64"}
    files
    {
        "%{BASE_DIR}external/jq/boost_context/jump_x86_64_ms_pe_masm.asm",
        "%{BASE_DIR}external/jq/boost_context/make_x86_64_ms_pe_masm.asm"
    }
    links
    {
        "winmm.lib"
    }
    filter {"files:**.asm"}
        buildaction "Masm"
    
    filter {}
end


function includeALL()
    includeVulkan()
    includeGLFW()
    includeFMOD()
    includeTracy()
    includeSTB()
    includeAssimp()    
    includeLua()
    includeSOL2()
    includeEnTT()
    includeCereal()
    includeGenerator()
    includeJq()
end

function linkALL()
    linkVulkan()
    linkGLFW()
    linkFMOD()
    linkXInput()
    linkAssimp()
    linkLua()
    linkGenerator()
end