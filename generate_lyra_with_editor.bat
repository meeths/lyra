@echo off
rem call scripts\generators\process_components.bat
echo -----------------------------------------------------------------------
echo [92mGenerating projects...[0m
tools\premake\premake5 vs2022 --file=lyra_with_editor.lua
echo [92mlyra code generation finished[0m