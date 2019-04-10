//
// StringEx.cpp
//
#include "stdafx.h"
#include "stringEx.h"

BOOL extractWordFollowingKeyword(LPCTSTR src, LPCTSTR szKeyword, LPCTSTR szSep, CString &csResult)
{
	csResult.Empty();

	CString csKey = szKeyword;					
	int iOff, iLen;
	CString a = src;

	// See if keyword exists in src
	if( a.Find(csKey) < 0)
		return FALSE;

	// Extract substring starting with keyword
	iLen = a.GetLength();
	iOff = a.Find(csKey);
	a = a.Right(iLen - iOff);

	// Trim-off the keyword
	iLen = a.GetLength();
	iOff = csKey.GetLength();
	a = a.Right(iLen - iOff); 

	// Trim-off any leading whitespace or colon
	a.TrimLeft(szSep);
	if(a.IsEmpty())
		return TRUE;

	// Find first whitespace to delimit end of the keyValue
	iOff = a.FindOneOf(szSep);
	if(iOff > -1)
	{
		// Extract Left up to that first whitespace
		a = a.Left(iOff);
		a.TrimRight(szSep);
	}
	csResult = a;
	return TRUE;
}


BOOL getCmdLineOpt(LPCTSTR keyword, CString &opt)
{
	CString strCmdLine = AfxGetApp()->m_lpCmdLine;  
	LPCTSTR szSep = _T(": \t\r\n");
	return extractWordFollowingKeyword(strCmdLine, keyword, szSep, opt);
}

#ifdef _UNICODE
void strNcpy(LPTSTR dst, LPCTSTR src, int maxNch)
{
	_tcsncpy(dst,src,maxNch);
	dst[maxNch -1] = 0;
}
#endif

extern "C"
{
	void strNcpy(char *dst, const char *src, int max)
	{
		ASSERT(src);
		ASSERT(dst);
		strncpy(dst,src,max);
		dst[max -1] = 0;
	}
}

LPCTSTR formatTimeConcisely(time_t t)
{
	CTime c(t);

	static UINT uiLocaleIdShortMonthName[] = {
		LOCALE_SABBREVMONTHNAME1,  // Jan
		LOCALE_SABBREVMONTHNAME2,  
		LOCALE_SABBREVMONTHNAME3,
		LOCALE_SABBREVMONTHNAME4,
		LOCALE_SABBREVMONTHNAME5, 
		LOCALE_SABBREVMONTHNAME6, 
		LOCALE_SABBREVMONTHNAME7,
		LOCALE_SABBREVMONTHNAME8,
		LOCALE_SABBREVMONTHNAME9,
		LOCALE_SABBREVMONTHNAME10,
		LOCALE_SABBREVMONTHNAME11,
		LOCALE_SABBREVMONTHNAME12,

		LOCALE_SABBREVMONTHNAME13	// #define LOCALE_SABBREVMONTHNAME13     0x0000100F   // abbreviated name for 13th month (if exists)

	};

	TCHAR strMonthName[MAX_PATH];
	_tcscpy(strMonthName, _T("<mon>"));
	int k = c.GetMonth() -1;
	::GetLocaleInfo(LOCALE_USER_DEFAULT, uiLocaleIdShortMonthName[k], strMonthName, sizeof(strMonthName));

	static CString a;
	a.Format(_T("%02d %s %02d %02d:%02d:%02d"),
		c.GetDay(),strMonthName,c.GetYear()%100,c.GetHour(),c.GetMinute(), c.GetSecond());
	return a;
}


void formatTimeAsHHMMSS(unsigned uiSeconds, CString &hh, CString &mm, CString &ss)
{
	
	// Format duration as hh:mm:ss
	
	int hours = uiSeconds / 3600;
	uiSeconds -= (hours * 3600);
	int min = uiSeconds / 60;
	uiSeconds -= (min * 60);
	int sec = uiSeconds;
	
	hh.Format("%d",  hours);
	mm.Format("%d",  min);
	ss.Format("%d",  sec);
	
}

unsigned decodeTimeFromHHMMSS(LPCTSTR hh, LPCTSTR mm, LPCTSTR ss)
{
	unsigned hr  = atoi(hh);
	unsigned min = atoi(mm);
	unsigned sec = atoi(ss);
	unsigned totalSeconds = ((hr * 60) + min) * 60 + sec;
	return totalSeconds;
	
}


// EOF
