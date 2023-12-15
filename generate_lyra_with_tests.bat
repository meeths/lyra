@echo off
rem call scripts/generators/process_components.bat
echo -----------------------------------------------------------------------
echo Generating projects
tools\premake\premake5 vs2022 --file=lyra_with_tests.lua