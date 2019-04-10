//
// StringEx.h
//
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	void strNcpy(char *dst, const char *src, int max);


#ifdef __cplusplus
} // extern "C" {
#endif

#ifdef _UNICODE 

void strNcpy(LPTSTR dst, LPCTSTR src, int maxNch);


#endif


#if defined _WIN32 && defined __cplusplus

BOOL getCmdLineOpt(LPCTSTR keyword, CString &opt);	// Use ":" as a separator, so "+key opt" and "+key: opt" are equipotent
BOOL extractWordFollowingKeyword(LPCTSTR src, LPCTSTR szKeyword, LPCTSTR szSep, CString &csResult);
	// Returns the white-space delimited "word" following "szKeyword"
	// The delimitors are given by "szSep"
	// There need not be any delimitors between szKeyword and the value,
	// Thus the following are equivalent:
	//	+myKeyMyVal
	//	+myKey:MyVal
	//	+myKey MyVal
	//	+myKey: MyVal


		
LPCTSTR formatTimeConcisely(time_t);		// This will respect the user's locale.  The order is always dd monthName yy hh:mm:ss, e.g. "11 Sep 01 08:20:14"
		// The names-of-months will be taken from Windows Locale for abbreviated month names in local language.


void     formatTimeAsHHMMSS(unsigned uiSeconds, CString &hh, CString &mm, CString &ss);
unsigned decodeTimeFromHHMMSS(LPCTSTR hh, LPCTSTR mm, LPCTSTR ss);

#endif 

/*#endif*/ // _STRING_EX_H_H_H
