@echo off
mkdir ..\..\..\output\share\CodeBlocks\plugins > nul 2>&1
copy ..\..\..\devel\share\CodeBlocks\plugins\codesnippets.exe ..\..\..\output\share\CodeBlocks\plugins\codesnippets.exe > nul 2>&1
strip ..\..\..\output\share\CodeBlocks\plugins\codesnippets.exe > nul 2>&1
zip -j9 ..\..\..\devel\share\CodeBlocks\codesnippets.zip manifest.xml
