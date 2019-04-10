//
// msg.h
//
#pragma once

void   msgCEdit(CEdit *e, const TCHAR *a);
void   msgCEdit_Clear(CEdit *e);
CEdit * msgSetStdout(CEdit *);	// Return previous CEdit, or NULL


//////////////////////////////////////////////////////////////////////////
// Diagnostic Levels
//////////////////////////////////////////////////////////////////////////
#ifndef eDiagLevel_Defined_
#define eDiagLevel_Defined_
typedef enum eDiagLevelTagEx
{
	// All levels go to all hooks; hooks now get the level as an arg.  Figure it
	DiagLevelLog = 0,		// Log file,
	DiagLevelInfo,			// Log file, 
	DiagLevelNotice,
	DiagLevelWarn,			// Log file, 
	DiagLevelError,			// Log file, 
	DiagLevelFatal			// All that stuff, then we call _exit(-1), so there!
} eDiagLevel;

#endif // eDiagLevel_Defined_


void msg(const char *a, const char *b = NULL,
	const char *c = NULL, 
	const char *d = NULL, 
	const char *e = NULL,
	const char *f = NULL);

void msg(const char *a, int b);
void msg(const char *a, long b);
void msg(const char *a, unsigned b);
void msg(const char *a, DWORD b);
void msg(const char *a, float b);
void msg(const char *a, double b);

void warn(const char *a, int b);
void warn(const char *a, long b);
void warn(const char *a, unsigned b);
void warn(const char *a, DWORD b);
void warn(const char *a, float b);
void warn(const char *a, double b);





void warn(const char *a, const char *b = NULL,
	const char *c = NULL, 
	const char *d = NULL, 
	const char *e = NULL,
	const char *f = NULL);

void diag(eDiagLevel lvl, const char *a, const char *b = NULL,
	const char *c = NULL, 
	const char *d = NULL, 
	const char *e = NULL,
	const char *f = NULL);

