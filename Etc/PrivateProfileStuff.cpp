//
// PrivateProfileStuff.cpp
//
#include "stdafx.h"
#include "math.h"
#include "PrivateProfileStuff.h"
#include "msg.h"
#include "fnsplit.h"
#include "direct.h"
#include "StringEx.h"

//#include "InsDir.h"		// Only used for a bad default, following an ASSERT


static CString s_path;
static CString s_sectionName;
static DWORD s_dwOpts = 0;
 

void setPrivateProfIniFilePath(const char *path, DWORD dwOpts)
{

//	ASSERT(strlen(path) < 100); // Could be longer, but catch wild cards
	ASSERT(strlen(path) < MAX_PATH);

	setPrivateProfOptions(dwOpts);
	setPrivateProfIniFilePath(path);
	makeDirAndUpperDirs(path);
}

void setPrivateProfIniFilePath(const char *path)
{
//	ASSERT(strlen(path) < 100); // Could be longer, but catch wild cards
	ASSERT(strlen(path) < MAX_PATH);

	s_path = path;

	if ( (s_dwOpts & PrivateProfOpt_SendToLogFileAlways) || (s_dwOpts & PrivateProfOpt_SendToLogFileIfNotDefault))
		msg("PRIVATE PROFILE FILE:",path);
}

void setPrivateProfIniFileSection(const char *section)
{
	s_sectionName = section;
	if ( (s_dwOpts & PrivateProfOpt_SendToLogFileAlways) || (s_dwOpts & PrivateProfOpt_SendToLogFileIfNotDefault))
		msg("SECTION:",section);
}

void setPrivateProfOptions(DWORD dwOpts)
{
	s_dwOpts = dwOpts;
}

LPCTSTR getPrivateProfStr(const char *key, LPCTSTR dflt,	const char *sectionName)
{
	if(!sectionName)
		sectionName = s_sectionName;
	ASSERT(sectionName);

	char *illegalValue = "THIS_IS_AN_UNUSUAL_AND_172338_ILLEGAL_VALUE";
	static char buf[MAX_PATH];
	int bufsz = MAX_PATH;
	buf[0] = 0;
#ifdef _DEBUG 
	char *ptr = buf;
#endif

	if(!dflt) dflt = "";

	if(!s_path)
	{
		ASSERT(0);
		return NULL;
		// No, to clever by half:
		// s_path = getInsDir_VccSysFile("General.ini");  // #ifdef _GetInsDir use getInsDir_Common()
	}
	makeDirAndUpperDirs(s_path);
	

	BOOL bTookDefault=FALSE;

	// "GetPrivateProfileString()" seems to return the default even
	// if the key does not exist; it does NOT seem to create the key in this case!
	// So we provide a default that is easy to spot (we hope)
	DWORD nch = GetPrivateProfileString(sectionName,key,illegalValue,buf,bufsz,s_path);
	if(!nch || !strcmp(buf,illegalValue))
	{
		if(s_dwOpts & PrivateProfOpt_AddMissingEntries)
		{
			{
				CString csDir,csFile,csExt,csPath;
				fnsplit(s_path,csDir,csFile,csExt);
				_mkdir(csDir);
			}
			BOOL b = WritePrivateProfileString(sectionName,key,dflt,s_path);
			ASSERT(b);
		}
		bTookDefault = TRUE;
		GetPrivateProfileString(sectionName,key,dflt,buf,bufsz,s_path);
	}

	BOOL bSendToLog = s_dwOpts & PrivateProfOpt_SendToLogFileAlways;
	if( !bTookDefault && (s_dwOpts & PrivateProfOpt_SendToLogFileIfNotDefault))
		bSendToLog = TRUE;

	if(bSendToLog)
	{
		CString a;
		a.Format("  %s=%s (%s)",key,buf,bTookDefault ? "Default" : "From File");
		msg(a);
	}
	return buf;
}

void setPrivateProfStr(const char *key, LPCTSTR val,	const char *sectionName)
{
	if(!sectionName)
		sectionName = s_sectionName;
	ASSERT(sectionName);

	BOOL b = WritePrivateProfileString(sectionName,key,val,s_path);
	ASSERT(b);

}

int getPrivateProfInt(const char *key, int idflt, const char *sectionName )
{	
	CString csDflt;
	csDflt.Format("%d",idflt);
	
	LPCTSTR a = getPrivateProfStr(key,csDflt,sectionName);
	if(!a)
	{
		ASSERT(0);
		return 0;
	}
	int k = atoi(a);
	return k;
}

int getPrivateProfInt(const char *key, int *pval, int idflt, const char *sectionName )
{
	int val = getPrivateProfInt(key, idflt, sectionName);
	if(pval) *pval = val;
	return val;
}


void setPrivateProfInt(const char *key, int x, const char *sectionName )
{
	CString a;
	a.Format("%d", x);
	setPrivateProfStr(key,a,sectionName);
}



DWORD getPrivateProfDWORD(const char *key, DWORD idflt, const char *sectionName )
{	
	CString csDflt;
	csDflt.Format("0x%x",idflt);
	
	LPCTSTR a = getPrivateProfStr(key,csDflt,sectionName);
	if(!a)
	{
		ASSERT(0);
		return 0;
	}
	DWORD k = 0;

	int n = sscanf(a,"0x%x",&k);
	if(!n)
		n = sscanf(a,"0X%x",&k);
	
	return k;
}

UCHAR   getPrivateProfUCHAR(const char *key, UCHAR dflt, const char *sectionName )
{
	CString csDflt;
	csDflt.Format("0x%x", (UINT)dflt);

	LPCTSTR a = getPrivateProfStr(key,csDflt,sectionName);
	if(!a)
	{
		ASSERT(0);
		return 0;
	}
	DWORD k = 0;

	int n = sscanf(a,"0x%x",&k);
	if(!n)
		n = sscanf(a,"0X%x",&k);

	return (UCHAR)k;
}

void    setPrivateProfUCHAR(const char *key, UCHAR val, const char *sectionName )
{
	CString a;
	a.Format("0x%x", (UINT)val);
	setPrivateProfStr(key,a,sectionName);
}


DWORD getPrivateProfDWORD(const char *key, DWORD *pval, DWORD idflt, const char *sectionName )
{	
	DWORD val = getPrivateProfDWORD(key, idflt, sectionName);
	if(pval) *pval=  val;
	return val;
}


void setPrivateProfDWORD(const char *key, DWORD x, const char *sectionName )
{
	CString a;
	a.Format("0x%x", x);
	setPrivateProfStr(key,a,sectionName);
}


BOOL getPrivateProfBool(const char *key, BOOL bdflt, const char *sectionName )
{	
	int iDefault = bdflt ? 1 : 0;
	int k = getPrivateProfInt(key,iDefault,sectionName);
	return k ? TRUE : FALSE;
}

BOOL getPrivateProfBool(const char *key, BOOL *pval, BOOL bdflt, const char *sectionName )
{	
	BOOL val = getPrivateProfBool(key, bdflt, sectionName);
	if(pval) *pval=  val;
	return val;
}


void setPrivateProfBool(const char *key, BOOL bVal, const char *sectionName )
{
	int iVal = bVal ? 1 : 0;
	setPrivateProfInt(key, iVal, sectionName);
}

float	getPrivateProfFloat(const char *key, float xdflt, const char *sectionName )
{	
	CString csDflt;
	csDflt.Format("%.6f",xdflt);

	LPCTSTR a = getPrivateProfStr(key,csDflt,sectionName);
	double x = atof(a);
	return (float)x;
}

float getPrivateProfFloat(const char *key, float *pval, float xdflt, const char *sectionName )
{	
	float val = getPrivateProfFloat(key, xdflt, sectionName);
	if(pval) *pval=  val;
	return val;
}


void setPrivateProfFloat(const char *key, float x, const char *sectionName )
{	
	CString csVal;
	csVal.Format("%.8f",x);
	setPrivateProfStr(key, csVal, sectionName);
}

double	getPrivateProfDouble(const char *key, double xdflt, const char *sectionName )
{	
	CString csDflt;
	csDflt.Format("%.12lf",xdflt);
	
	LPCTSTR a = getPrivateProfStr(key,csDflt,sectionName);
	double x = atof(a);
	return x;
}

double	getPrivateProfDouble(const char *key, double *pval, double xdflt, const char *sectionName )
{	
	double val = getPrivateProfDouble(key, xdflt, sectionName);
	if(pval) *pval=  val;
	return val;
}

void	setPrivateProfDouble(const char *key, double x, const char *sectionName )
{
	CString csVal;
	csVal.Format("%.16f",x);
	setPrivateProfStr(key, csVal, sectionName);
}

void accPrivateProfBool(const char *tag, BOOL *pVal, int bSet)
{
	BOOL val = *pVal;
	if(bSet)
		setPrivateProfBool(tag, val);
	else
		*pVal = getPrivateProfBool(tag, val);
}

void accPrivateProfDWORD(const char *tag, DWORD *pVal, int bSet)
{
	DWORD val = *pVal;
	if(bSet)
		setPrivateProfDWORD(tag, val);
	else
		*pVal = getPrivateProfDWORD(tag, val);
}

void accPrivateProfDWORD(const char *tag, unsigned *pVal, int bSet)
{
	accPrivateProfDWORD(tag, (DWORD *)pVal, bSet);
}

void accPrivateProfUCHAR(const char *tag, UCHAR *pVal, int bSet)
{
	UCHAR val = *pVal;
	if(bSet)
		setPrivateProfUCHAR(tag, val);
	else
		*pVal = getPrivateProfUCHAR(tag, val);
}

void accPrivateProfInt(const char *tag, int *pVal, int bSet)
{
	int val = *pVal;
	if(bSet)
		setPrivateProfInt(tag, val);
	else
		*pVal = getPrivateProfInt(tag, val);
}

void accPrivateProfFloat(const char *tag, float *pVal, int bSet)
{
	float val = *pVal;
	if(bSet)
		setPrivateProfFloat(tag, val);
	else
		*pVal = getPrivateProfFloat(tag, val);
}

void accPrivateProfDouble(const char *tag, double *pVal, int bSet)
{
	double val = *pVal;
	if(bSet)
		setPrivateProfDouble(tag, val);
	else
		*pVal = getPrivateProfDouble(tag, val);
}


void accPrivateProfStr(const char *tag, CString &csVal, int bSet)// note that for "get" the passed param csVal is the default
{
	if(bSet)
		setPrivateProfStr(tag, csVal);
	else
		csVal = getPrivateProfStr(tag, csVal); // note that for "get" the passed param csVal is the default
}

void accPrivateProfCptr(const char *tag, char **pptr, int bSave)
{
	if(bSave)
	{
		CString aa = *pptr;
		accPrivateProfStr(tag,aa,bSave);
	}else{
		CString aa;
		accPrivateProfStr(tag,aa,bSave);
		if(*pptr)
		{
			free(*pptr);
		}
		*pptr = _strdup(aa);
	}
}

void accPrivateProfCharArray(const char *tag, char *pptr[], int bSave)
{
	if(bSave)
	{
		CString aa = *pptr;
		accPrivateProfStr(tag,aa,bSave);
	}else{
		CString aa;
		accPrivateProfStr(tag,aa,bSave);
		if(*pptr)
		{
			free(*pptr);
		}
		*pptr = _strdup(aa);
	}
}

void accPrivateProfChar(const char *tag, char *buf, int maxLen, int bSave)
{
	CString aa = buf;
	accPrivateProfStr(tag,aa,bSave);
	
	if(!bSave)
	{
		strNcpy(buf,aa, maxLen);
	}
}

void accPrivateProfLONGLONG(const char *tag, LONGLONG *pVal, int bSet)
{
	union {
		LONG l[2];
		LONGLONG ll;
	} u;

	if(bSet)
	{
		u.ll = *pVal;
		CString aa;
		aa.Format("%d_%d", u.l[0], u.l[1]);
		setPrivateProfStr(tag, aa);
	}else{
		CString aa = getPrivateProfStr(tag);
		int n = sscanf(aa,"%d_%d", &u.l[0], &u.l[1]);
		ASSERT(n==2);
		*pVal = u.ll;
	}
}
void accPrivateProfULONGLONG(const char *tag, ULONGLONG *pVal, int bSet)
{

	union {
		ULONG l[2];
		ULONGLONG ll;
	} u;

	if(bSet)
	{
		u.ll = *pVal;
		CString aa;
		aa.Format("%x_%x", u.l[0], u.l[1]);
		setPrivateProfStr(tag, aa);
	}else{
		CString aa = getPrivateProfStr(tag);
		int n = sscanf(aa,"%x_%x", &u.l[0], &u.l[1]);
		ASSERT(n==2);
		*pVal = u.ll;
	}

}