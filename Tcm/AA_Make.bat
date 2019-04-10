@echo off
call ..\..\..\AA_SetProject.bat
del /S *.lastbuildstate
set Tgt_Dir=Sample_Eagle_App\\TCM
set Tgt_Sln=_SampleEagleApp_Tcm
set Tgt_Configs="Debug|x64" "Release|x64"
call \Src\Src_Common\Bin\AAA_Vs10_Make.bat
if  NOT _%BATCH% == _YES pause


