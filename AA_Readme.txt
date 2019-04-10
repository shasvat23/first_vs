August 11, 2017

Sample Eagle App

Goal:
  Demonstrate programming and administration for a simple "Eagle" app,
this app has a GUI (on the Win10 Gui Host) and a Test-control-module ("TCM")
on the Eagle Server.

Names:
  The names of the GUI and the TCM (Test control module) must be related by adding "_Tcm"
  Gui Name:   SampleEagleApp.exe
  Tcm Name:   SampleEagleApp_Tcm.exe

32/64 bit
  We're 64-bit.

Unicode/MBCS
  We're  MBCS

Directories:
  \DebugBin\SampleEagleApp\	
    This is the output directory for the build-processes for GUI and TCM
    This is the run-time dir for debugging -- so it must contain debug versions
    of these support dlls:
	NiDevSupport64D.dll
	-> This dll allows the GUI to access some NIDaqMx functions;
	-> The GUI does NOT need to link to LibNiDaqMx
	-> If the gui is run on a machine that does not have DaqMx installed that is not an error
	-> If the gui is intended to run on either Hawk or Eagle, then it will use
	-> functions in NiDevSupport64.dll to query the driver to see what boards are installed,
	-> and get their serial numbers, and then it can check the license keyes -- this logic
	-> is "Hawk-Only" -- on Eagle, all that is done on the TCM on the Eagle.


What does the GUI do?
  It has a button "Echo Request"; if you click it,
  it sends a request to its TCM to echo-back something.

What is in common between GUI and TCM
 Message structs and message id's
  ..\Etc\AppMsgs.h
  This includes:
   -> Message structs -- every one of them starts with an "int msgId"
   -> Message ids -- the first int in a message
   -> A "magic number" that might make debugging easier -- shared by "Client" and "Server" codes

What are the major modules of the GUI
  main app -- "Gui.cpp" -- this is a trivial MFC "Single dialog" app.
  main dlg -- "GuiDlg.cpp" -- this is a trivial MFC dialog that manages the "Eagle Client"


  Eagle "Client"
	CSampleAppClient

  Code to send echo request
	void CGuiDlg::OnBnClickedOk()

  Code to handle the echo reply
	void CGuiDlg::dispatchMsgFromTcm(char *buf, size_t sz)



What are the major modules of the TCM
  main app
  main dlg
  Eagle "Server"
	CSampleAppSvr

  Code to handle requests from the client
	void CGuiDlg::dispatchMsgFromClient(int iClientId, DWORD_PTR dwApplicationLevelClientData, void *buf, size_t sz)


  Code to handle windows shutdown requests
	CGuiDlg::OnQueryEndSession(void)
	CGuiDlg::OnEndSession(BOOL bEndingNowFolks)

What's it going to take to compile this?
  /Src/Src_Eagle_Millenium/Src/Sample_Eagle_App


What does it take to install this?

  TCM must be installed into GUI machine at 
  C:\Users\Public\Documents\MB Dynamics\VibControlCommon\Embedded\Win7\
  All .exe and .dll that you place in this directory will be copied
  automagically to <Eagle Server Machine>:C:\Program Files (x86)\MB Dynamics\MilleniumEagle\

  Desktop launcher: This is not an exhaustive essay on the launcher.
  Just use the "Miso1" launcher and "MB_Miso1" directory and "Miso1 site support" 
  as a starting point; edit the launcher ini file to add your new app.


What other dll's -- the Eagle base software should include all the run-time dll's
 
