set BATCH=YES

pushd Gui
  call AA_Make.bat
popd

pushd Tcm
  call AA_Make.bat
popd

@echo Done with make in Sample_Eagle_App
pause



