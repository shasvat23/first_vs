//
// Fnsplit.cpp
//
#include "stdafx.h"
#include "fnsplit.h"
#include "direct.h"

#ifdef _SINE 
#include "SineGlobals.h"
#define GetCommandLineEx GetCommandLineSine
#else
#define GetCommandLineEx GetCommandLine
#endif

#if 0
void getcwd(char *buf, int sz)
{
	_getcwd(buf,sz);
}
#endif
 
void fnsplit(LPCTSTR aPath, CString & cstrDir, CString & cstrFile, CString & cstrExt)
{
	// INPUT: cstrPath as d:\src\Mainfrm.cpp
	// Output
	//		cstrDir = d:\Src
	//		cstrFile = Mainfrm
	//		cstrExt = cpp

	// Hint: CString.ReverseFind(TCHAR) returns array index of first instance of char (reverse search) 

	CString cstrPath = aPath;
	CString cstrTmp;
	int n = cstrPath.GetLength();
	cstrDir.Empty();
	cstrFile.Empty();
	cstrExt.Empty();
	if(n < 1)
		return;

	int idx = cstrPath.ReverseFind('.');
	if(idx > -1)
	{
		// Pathological cases, I.  Ends in a '.'
		if(idx == cstrPath.GetLength() -1)
			idx = -1;

		// Pathological cases, II. There is no extension, and the path contains a "./" or ".\"
		if(idx > -1)
		{
			TCHAR next = cstrPath.GetAt(idx + 1);
			if(next == _T('/') || next == _T('\\'))
				idx = -1;
		}

	}
	if(idx > -1)
	{
		cstrExt = cstrPath.Right(n - idx -1);	// Or is it idx+1
		cstrTmp = cstrPath.Left(idx);		// Or is it idx-1 ?
	}else{
		cstrExt.Empty();
		cstrTmp = cstrPath;
	}
	idx = cstrTmp.ReverseFind('\\');
	if(idx < 0)
		idx = cstrTmp.ReverseFind('/');
	if(idx > -1)
	{	
		n = cstrTmp.GetLength();
		cstrFile = cstrTmp.Right(n - idx -1);
		cstrDir = cstrTmp.Left(idx);

	}else{
		cstrDir.Empty();
		cstrFile = cstrTmp;
	}

}

void fnmerge(CString &cstrPath, LPCTSTR cstrDir, LPCTSTR  cstrFile, LPCTSTR  cstrExt)
{
	cstrPath.Empty();
	if(cstrDir)
	{
		if(cstrDir[0])
		{
			cstrPath = cstrDir;
			cstrPath += _T("\\");
		}
	}

	CString csExt = cstrExt;
	csExt.TrimLeft(".");
	cstrPath += cstrFile;
	cstrPath += _T(".");
	cstrPath += csExt;
}

void fnSetSuffix(LPCTSTR src, LPCTSTR sfx, CString &dst)
{
	// sfx may be either ".xxx" or just "xxx"
	if(sfx[0] == _T('.'))
		sfx++;

	CString cstrPath, cstrDir, cstrFile, cstrExt;
	cstrPath = src;
	fnsplit(cstrPath, cstrDir, cstrFile, cstrExt);
	cstrExt = sfx;
	fnmerge(cstrPath,cstrDir,cstrFile,cstrExt);
	dst = cstrPath;

}

BOOL bFileExists(LPCTSTR path)
{
	if(!path)
		return FALSE;
	if(!*path)
		return FALSE;
	if(bIsaDir(path))
		return FALSE;
	FILE *f = _tfopen(path,_T("rb"));
	if(f)
	{
		fclose(f);
		return TRUE;
	}
	return FALSE;
}


#include "sys\Stat.h"
BOOL bFileIsRdonly(LPCTSTR path)
{
	if(!path)
		return FALSE;
	if(!*path)
		return FALSE;
	if(!bFileExists(path))
		return FALSE;
	
	DWORD dwAttr = GetFileAttributes(path);
	if(dwAttr == INVALID_FILE_ATTRIBUTES)
		return 0;

	int bIsNormal = (dwAttr & FILE_ATTRIBUTE_NORMAL); // just checking

	if(dwAttr & FILE_ATTRIBUTE_READONLY)
		return 1;

	return 0;
}
 
BOOL bIsaDir(LPCTSTR path)
{
	if(!path)
		return FALSE;
	if(!*path)
		return FALSE;

	DWORD dwAttr = GetFileAttributes(path);
	if(dwAttr == INVALID_FILE_ATTRIBUTES)
		return 0;
	

	if(dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		return 1;

	return 0;
}

BOOL bFileExistsInDir(const char *name, const char *dir)
{
	CString slash = "\\";
	CString csName = name;
	CString csDir = dir;
	CString path;
	csName.TrimRight(slash);
	csDir.TrimRight(slash);
	path = csDir + slash + csName;
	return bFileExists(path);
}

BOOL FileExistsInAnyFirstLevelSubDir(const char *dirStart, const char *szName, CString &csSubDir)
{
	CString slash = "\\";

	CString csDirStart = dirStart;
	csDirStart.TrimRight(slash);

	CString csSearch = csDirStart + "\\*.*";

	WIN32_FIND_DATA FindData;
	HANDLE hFile = FindFirstFile(csSearch, &FindData);
	if(hFile == NULL || hFile == INVALID_HANDLE_VALUE)
	{
		ASSERT(0);
		return 0;
	}

	while( FindNextFile(hFile, &FindData))
	{
		//TRACE("FILE IN MB: %s\n", FindData.cFileName);
		CString csFile = FindData.cFileName;
		if(csFile == "." || csFile == "..")
			continue;
		if ( ! ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;
		CString csSubDirSearch = csDirStart + slash + csFile;
		if(bFileExistsInDir(szName, csSubDirSearch))
		{
			csSubDir = csSubDirSearch;
			return TRUE;
		}
	}

	int kkk=0;
	return 0;
}

#include "stdlib.h" // for errno 
#include "errno.h"

BOOL makeDirAndUpperDirs(LPCTSTR aszPath, BOOL bIsaDir /* = FALSE */)	
  // e.g. for c:\fu\bar create c:\fu then c:\fu\bar
 // But for c:\Fu\Fubar.dat only create c:\Fu
 // Unless bIsaDir is given, then for c:\Fu\Fubar.May18 create c:\Fu and c:\Fubar.May18
{
	CString csPath = aszPath;
	csPath.TrimLeft(" \r\n\t");
	csPath.TrimRight(" \r\n\t");
	if(csPath.IsEmpty())
		return TRUE;

	// Does it start with "/" or "\\" ?? If so, this is significant;
	// if you do not worry about this now, this leading delimiter would
	// get stripped by strtok!
	char a1 = csPath[0];
	BOOL bLeadingSlash = a1 == '/' || a1 == '\\';
	if(bLeadingSlash)
		csPath.TrimLeft("/\\");

	// Determine whether to treat the path as a dir/file or as a pure dir
	CString dir,file,ext;
	fnsplit(csPath,dir,file,ext);

	BOOL bbIsaDir = ext.IsEmpty() || bIsaDir;
	

	// If treating it as a pure dir, then csDir = szPath, the originally given path,
	// elsewise, it's just the "dir" section we split off via fnsplit
	CString csDir = bbIsaDir ? csPath : dir;

	char *buf = _strdup(csDir);
	char *sep = "/\\"; // Wigwams anyone?
	CString base;
	char *tok;
	int iret;
	BOOL bstat = 0;
	if(bLeadingSlash)
		base = "\\";
	for(tok = strtok(buf,sep); tok ; tok = strtok(NULL, sep))
	{
		base += tok;
	
		if(tok[1] != ':') // Silly bear! it's c:\....
		{
			iret = _tmkdir(base);
			bstat = (iret == 0 || errno == EEXIST);
		
			if(!bstat)
			{
				int kkk=errno;
				int jj = EEXIST; // ENOENT is 2
				// break; // Keep Clam
			}
		}

		base += "\\";
	}

	if(base.IsEmpty())
	{
		// did not find any / or \\ in buf 
		iret = _tmkdir(csDir);
		bstat  = (iret == 0 || errno == EEXIST);
		if(!bstat)
		{
			int kkk=0;
		}
	}

	free(buf);
	return bstat;
}

BOOL makeDirAndUpperDirs_C(const char * szPath)
{
	return makeDirAndUpperDirs(szPath);
}

extern "C" int ArgGiven_c(const char *key)
{
	return ArgGiven(key, NULL);
}

#if 1 // This version of "ArgGiven" does NOT match initial substrings

BOOL ArgGiven(LPCTSTR keyword, LPCTSTR cmdLine)
{
	CString cmd = cmdLine ? cmdLine : GetCommandLineEx();
	cmd.TrimRight("\r\n\t");
	cmd += " ";
	CString tgt = keyword;
	tgt += " ";
	cmd.MakeLower();
	tgt.MakeLower();
	int k = cmd.Find(tgt);
	return (k > -1);
}

BOOL ArgGivenEx(LPCTSTR keys[])
{
	for(int i=0;i<9999;i++)
	{
		LPCTSTR k = keys[i];
		if(!k)
			return FALSE;
		if(ArgGiven(k))
			return TRUE;
	}
	ASSERT(0);
	return FALSE;
}

BOOL ArgGivenEx(LPCTSTR key0, LPCTSTR key1, LPCTSTR key2, LPCTSTR key3, LPCTSTR key4, LPCTSTR key5)	// Return true iff any of these are given
{
	LPCTSTR keys[7];
	keys[0] = key0;
	keys[1] = key1;
	keys[2] = key2;
	keys[3] = key3;
	keys[4] = key4;
	keys[5] = key5;
	keys[6] = NULL;
	return ArgGivenEx(keys);
}


#else // This version of "ArgGiven" DOES match initial substrings

BOOL ArgGiven(LPCTSTR keyword, LPCTSTR cmdLine)
{
	CString cmd = cmdLine ? cmdLine : GetCommandLineEx();
	CString tgt = keyword;
	cmd.MakeLower();
	tgt.MakeLower();
	return (cmd.Find(tgt) > -1);
}
#endif 


BOOL iArgGiven(LPCTSTR keyword, int *piVal, LPCTSTR cmdLine)
{
	CString cmd = cmdLine ? cmdLine : GetCommandLineEx();
	CString tgt = keyword;
	cmd.MakeLower();
	tgt.MakeLower();
	int k = cmd.Find(tgt);
	if(k < 0)
	{
		// Given "/Apples"; did not find /Apples17, search for /Apples=17
		if(tgt.Find("=") < 0)
		{
			tgt.TrimRight(" \r\n\t");
			tgt += "=";
			return iArgGiven(tgt, piVal);
		}else{
			return FALSE;
		}
	}
		
		
	LPCTSTR ptr = (LPCTSTR)cmd+k+strlen(tgt);
	if(*ptr == '=')
		ptr++;
	k = atoi(ptr);
	if(piVal)
		*piVal = k;
	return TRUE;
}

BOOL xArgGiven(LPCTSTR keyword, unsigned *piVal, LPCTSTR cmdLine)
{
	// If a="/Apples" it will return TRUE  /Apples=0x17 -- Note the "0x" prefix is required

	CString cmd = cmdLine ? cmdLine : GetCommandLineEx();
	CString tgt = keyword;
	cmd.MakeLower();
	tgt.MakeLower();
	int k = cmd.Find(tgt);
	if(k < 0)
		return FALSE;
	
	LPCTSTR ptr = (LPCTSTR)cmd+k+strlen(tgt);
	if(*ptr == '=')
		ptr++;

	int n = sscanf(ptr, "0x%x", piVal);
	
	return (n==1);
}



#if 1 // new strArgGiven for double-quote protected whitespace

BOOL strArgGiven(LPCTSTR keyword, CString &b, LPCTSTR cmdLine)
{
	// For tgt= /Apples=" and cmd = /Apples=MacIntosh it returns TRUE and b=MacIntosh
	// Must have the "=" 
	// For /Apples="Mac and Cheese" return Mac and Cheese, e.g. strip the quotes and preserve the whitespace
	// keyword 'a' is case-insensitive but b will be return case-true

	CString cmd0 = cmdLine ? cmdLine : GetCommandLineEx();

	CString cmd = cmd0;
	CString tgt = keyword;
	cmd.MakeLower();	// "cmd" is lowercased, 'cmd0' retains case-truth
	tgt.MakeLower();
	int k = cmd.Find(tgt);
	if(k < 0)
	{
		b.Empty();
		return FALSE;
	}

	// Get "buf0" = cmd0, and then cloak quotes and quote-protected whitespace
	char *buf0 = _strdup(cmd0);
	int n0=(int)strlen(buf0);
	int bProt=0;
	char quote = '\"';	// And you can quote me on that one!
	char space = ' ';
	char quoteCloak = 1;
	char spaceCloak = 2;
	for(int i=0;i<n0;i++)
	{
		if(buf0[i] == quote)
		{
			bProt = 1-bProt;
			buf0[i] = quoteCloak;
		}
		if(bProt)
		{
			if(buf0[i] == space)
				buf0[i] = spaceCloak;
		}
	}

#ifdef _DEBUG
	CString a0 = buf0;
#endif 

	// Extract the target (it is cloaked and will contain quote-cloaks as well as white-space cloaks)
	LPCTSTR ptr = (LPCTSTR)buf0+k+strlen(tgt);
	char *buf = _strdup(ptr);
	int n = (int)strlen(buf);
	for(int i=0;i<n;i++)
	{
		if(buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\r' || buf[i] == '\n')
		{
			buf[i] = 0;
			break;
		}
	}

	// Undo the cloaking
	int n2 = (int)strlen(buf);
	for(int i=0;i<n2;i++)
	{
		if(buf[i] == spaceCloak)
			buf[i] = space;
		if(buf[i] == quoteCloak)
			buf[i] = quote;
	}

	// Trim off the now-uncloaked quote(s)
	b = buf;
	b.Trim(quote);
	b.TrimLeft(" =");
	b.Trim(quote);	// Not sure if this is needed
	b.Trim(space);  // Not sure if this is needed

	free(buf0);
	free(buf);

	return TRUE;
}

#else 
BOOL strArgGiven(LPCTSTR keyword, CString &b, LPCTSTR cmdLine)
{
	// For tgt= /Apples=" and cmd = /Apples=MacIntosh it returns TRUE and b=MacIntosh
	// Must have the "=" 
	// Must not have whitespaces
	// keyword 'a' is case-insensitive but b will be return case-true

	CString cmd0 = cmdLine ? cmdLine : GetCommandLineEx();
	
	CString cmd = cmd0;
	CString tgt = keyword;
	cmd.MakeLower();	// "cmd" is lowercased, 'cmd0' retains case-truth
	tgt.MakeLower();
	int k = cmd.Find(tgt);
	if(k < 0)
	{
		return FALSE;
	}
	
	
	LPCTSTR ptr = (LPCTSTR)cmd0+k+strlen(tgt);
	char *buf = _strdup(ptr);
	int n = strlen(buf);
	for(int i=0;i<n;i++)
		if(buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\r' || buf[i] == '\n')
			buf[i] = 0;
	b = buf;
	b.TrimLeft(" =");

	return TRUE;
}
#endif // V0


LPCTSTR addPathComponents(LPCTSTR path, LPCTSTR dirOrFile)
{
	CString slash = "\\";
	CString slashen = "/\\";
	CString whitespace = " \r\n\t";
	static CString csPath;
		
	csPath = path;
	csPath.Trim(whitespace);
	csPath.TrimRight(slashen);
	csPath += slash;
	csPath += dirOrFile;
	
	return csPath;
}


LPCTSTR addPathComponent(LPCTSTR path, LPCTSTR dirOrFile)
{
	return addPathComponents(path, dirOrFile);
}

BOOL bIsaFullyQualifiedPath(LPCTSTR path)
{
	if(!path)
		return 0;
	return (path[1] == ':');
}

__time64_t utcTime64(void * p)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	CTime ct(st);
	__time64_t t = ct.GetTime();
	return t;
}

__time32_t utcTime32(void *p)
{
	__time64_t t64 = utcTime64(p);
	return (__time32_t) t64;
}
__time64_t utcTime(void * p)
{
	return utcTime64(p);
}



LPCTSTR getWinAppExePath(void) // This implementation via GetStartupInfo
	// Return, for instance, "C:\DebugBin\6Dof\Mimo6a_Ctl_x64_dbg.exe"
{
	STARTUPINFOW StartupInfo;
	GetStartupInfoW( &StartupInfo );
	static CString cc;
	cc = StartupInfo.lpTitle; // cc = "C:\DebugBin\6Dof\Mimo6a_Ctl_x64_dbg.exe"
	return cc;
}

LPCTSTR GetMyImagePath(void) // This implementation via GetCommandLine, and parse
{
	LPCTSTR a0 = GetCommandLine();
	TCHAR * a1 = _tcsdup(a0);

	CString sep = "\"";
	char *ctx = 0;

	LPCTSTR a2 = _tcstok_s(a1, sep, &ctx);
	
	static CString szImagePath;

	szImagePath = a2;

	free(a1);

	return szImagePath;
}

LPCTSTR getExeFullPathFromSameDirAsMyself(LPCTSTR _tgtExeName)
{
	CString path,dir,name,ext;
	

	// Remove the *.exe if any
	fnsplit(_tgtExeName, dir,name,ext);
	CString tgtExeName = name;

	// Get my own full path
	CString myPath = getWinAppExePath();

	// Get my own exe directory from that path
	fnsplit(myPath, dir,name,ext);

	// Recombine to get shlim full path
	static CString tgtExePath;
	tgtExePath.Empty();	
	fnmerge(tgtExePath, dir,tgtExeName, ext);

	return tgtExePath;
}

#if 0
size_t getFileSize64(LPCTSTR path)
{
	FILE *f = fopen(path, "rb"); // In getFileFromServer, check final size and do throughput report
	if(!f)
		return 0;
	_fseeki64(f, 0, SEEK_END);
	size_t fileSz = _ftelli64(f);
	fclose(f);
	return fileSz;
}
#endif 

#include "sys\Stat.h"

// _CRTIMP int __cdecl _stat64(_In_z_ const char * _Name, _Out_ struct _stat64 * _Stat);
 
size_t getFileSize64(LPCTSTR path)
{
	struct _stat64 S;
	int iret = _stat64(path, &S);
	if(iret < 0)
		return 0;
//	ASSERT(sizeof(S.st_size) == sizeof(size_t)); // It is supposed to be __int64
	return (size_t)S.st_size;
	
}


// EOF
