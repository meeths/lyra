﻿---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by sisco.
---

function includeGLM()
    filter {}
    includedirs "%{BASE_DIR}external/glm"
    
    filter "configurations:Release"
        defines "GLM_FORCE_INLINE"
    filter {}
end