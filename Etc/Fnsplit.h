//
// Fnsplit.h
//
#pragma  once

//char * getcwd(char *buf, int sz);
#ifdef __cplusplus

void fnsplit(LPCTSTR cstrPath, CString & cstrDir, CString & cstrFile, CString & cstrExt);
void fnmerge(CString &cstrPath, LPCTSTR cstrDir, LPCTSTR  cstrFile, LPCTSTR  cstrExt);
	// INPUT: cstrPath as d:\src\Mainfrm.cpp
	// Output
	//		cstrDir = d:\Src
	//		cstrFile = Mainfrm
	//		cstrExt = cpp

void fnSetSuffix(LPCTSTR src, LPCTSTR sfx, CString &dst);
	// sfx may be either ".xxx" or just "xxx"

BOOL makeDirAndUpperDirs(LPCTSTR szDir, BOOL bIsaDir=FALSE);	// e.g. for c:\fu\bar create c:\fu then c:\fu\bar



BOOL bIsaDir(LPCTSTR path);
BOOL bFileExists(LPCTSTR path);
BOOL bFileIsRdonly(LPCTSTR path);
BOOL bFileExistsInDir(const char *name, const char *dir);
BOOL FileExistsInAnyFirstLevelSubDir(const char *dirStart, const char *szName, CString &csSubDir);

BOOL ArgGiven(LPCTSTR keyword, LPCTSTR cmdLine = NULL);
BOOL ArgGivenEx(LPCTSTR key1, LPCTSTR key2, LPCTSTR key3=NULL, LPCTSTR key4=NULL, LPCTSTR key5=NULL, LPCTSTR key6=NULL);	// Return true iff any of these are given
BOOL ArgGivenEx(LPCTSTR keys[]); // null-terminated list of keywords, return true iff any of them are given

BOOL iArgGiven(LPCTSTR keyword, int *piVal, LPCTSTR cmdLine = NULL);
	// If a="/Apples" it will return TRUE for either /Apples=17 or /Apples17

BOOL xArgGiven(LPCTSTR keyword, unsigned *piVal, LPCTSTR cmdLine = NULL);
// If a="/Apples" it will return TRUE  /Apples=0x17 -- Note the "0x" prefix is required

BOOL strArgGiven(LPCTSTR keyword, CString &b, LPCTSTR cmdLine = NULL);
	// for /Apples=MacIntosh it returns TRUE and b=MacIntosh
	// Must have the "=" 
	// Must not have whitespaces

LPCTSTR addPathComponents(LPCTSTR path,LPCTSTR dirOrFile);
LPCTSTR addPathComponent(LPCTSTR path,LPCTSTR dirOrFile);
BOOL bIsaFullyQualifiedPath(LPCTSTR path); // Return path[1] == ':'

LPCTSTR getExeFullPathFromSameDirAsMyself(LPCTSTR tgtExeName);
LPCTSTR getWinAppExePath();		// This implementation via GetStartupInfo
LPCTSTR GetMyImagePath(void);	// This implementation via GetCommandLine, and parse

#endif // ndef C++

#ifdef __cplusplus
extern "C" {
#endif 
	BOOL makeDirAndUpperDirs_C(const char * szPath);	// e.g. for c:\fu\bar create c:\fu then c:\fu\bar
	// e.g. for c:\fu\bar create c:\fu then c:\fu\bar
	// But for c:\Fu\Fubar.dat only create c:\Fu
	// Unless bIsaDir is given, then for c:\Fu\Fubar.May18 create c:\Fu and c:\Fubar.May18

	extern int ArgGiven_c(const char *key);

#ifdef __cplusplus
};
#endif 

// Replacements for time_t t = time(NULL), which gives local time, we want system time, which is in UTC.

__time64_t utcTime64(void *);
__time32_t utcTime32(void *);
__time64_t utcTime(void *);

size_t getFileSize64(LPCTSTR path);



