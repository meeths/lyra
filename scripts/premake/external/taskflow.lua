﻿---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by sisco.
--- DateTime: 16/12/2023 10:39
---

function includeTaskFlow()
    filter {}
    includedirs "%{BASE_DIR}external/taskflow/taskflow"
    defines "TF_DISABLE_EXCEPTION_HANDLING"
end

function compileTaskFlow()
    filter {}
end