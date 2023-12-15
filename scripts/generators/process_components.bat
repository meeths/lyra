@echo off
echo [92mProcessing lyra components[0m
echo ---------------------------------------------------------------------
echo [93mGenerating component keys...[0m
echo [93mOutput file: packages\lyra\include\ECS\EngineComponents.h[0m
echo.
py %~dp0\component_processor.py -d %~dp0\..\..\packages\lyra\include\ECS\CoreComponents -m %~dp0\..\..\packages\lyra\include\ECS\EngineComponents.h
echo ---------------------------------------------------------------------
echo [93mGenerating component serialization blocks[0m
echo [93mOutput file: packages\lyra\source\serializer\EntityComponentSerializer.cpp[0m
echo.
py %~dp0\component_processor.py -d %~dp0\..\..\packages\lyra\include\ECS\CoreComponents -j %~dp0\..\..\packages\lyra\source\ECS\Serializer\EntityComponentSerializer.cpp
