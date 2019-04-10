//
// PrivateProfileStuff.h
//
//	The Windoze API for private profile stuff has some subtleties that we'll here hide.
//
#ifndef _PRIVATE_PROFILE_STUFF_HH_H_H_H_
#define _PRIVATE_PROFILE_STUFF_HH_H_H_H_

void    setPrivateProfIniFilePath(const char *path);
void    setPrivateProfIniFilePath(const char *path, DWORD dwOpts);
void	setPrivateProfIniFileSection(const char *section);
void	setPrivateProfOptions(DWORD dwOpts);
#define PrivateProfOpt_AddMissingEntries		 0x01
#define PrivateProfOpt_SendToLogFileAlways		 0x02
#define PrivateProfOpt_SendToLogFileIfNotDefault 0x04
#define PrivateProfOpt_SendToLogFileNever		 0x00

LPCTSTR getPrivateProfStr(const char *key,	  LPCTSTR dflt = NULL,	const char *sectionName = NULL);
int     getPrivateProfInt(const char *key,	  int dflt = 0,			const char *sectionName = NULL);
int		getPrivateProfInt(const char *key,    int *pVal, int dflt, const char *sectionName = NULL);
DWORD   getPrivateProfDWORD(const char *key,  DWORD	 dflt = 0,			const char *sectionName = NULL);

UCHAR   getPrivateProfUCHAR(const char *key, UCHAR dflt = 0, const char *sectionName = NULL);
void    setPrivateProfUCHAR(const char *key, UCHAR val, const char *sectionName = NULL);

DWORD   getPrivateProfDWORD(const char *key,  DWORD *pVal, DWORD dflt, const char *sectionName = NULL);
BOOL	getPrivateProfBool(const char *key,	  BOOL dflt = FALSE,	const char *sectionName = NULL);
BOOL	getPrivateProfBool(const char *key,	  BOOL *pVal, BOOL dflt = FALSE,	const char *sectionName = NULL);
float	getPrivateProfFloat(const char *key,  float dflt = 0,		const char *sectionName = NULL);
float	getPrivateProfFloat(const char *key,  float *pVal, float dflt,		const char *sectionName = NULL);
double	getPrivateProfDouble(const char *key, double dflt = 0,		const char *sectionName = NULL);
double	getPrivateProfDouble(const char *key, double *pVal, double dflt,		const char *sectionName = NULL);

void setPrivateProfStr(const char *key, LPCTSTR val,	const char *sectionName = NULL);
void setPrivateProfInt(const char *key, int x, const char *sectionName = NULL);
void setPrivateProfDWORD(const char *key, DWORD x, const char *sectionName = NULL);
void setPrivateProfBool(const char *key, BOOL bVal, const char *sectionName = NULL);
void setPrivateProfFloat(const char *key, float x, const char *sectionName = NULL);
void setPrivateProfDouble(const char *key, double x, const char *sectionName = NULL);

// 2011 0519	We now have a single set of routines that will either set or get stuff.
//	So they are "accessors".  This allows the source code to refer to the "tags" only once
void accPrivateProfDWORD(const char *tag, DWORD *pVal, int bSet);
void accPrivateProfDWORD(const char *tag, unsigned *pVal, int bSet);

void accPrivateProfUCHAR(const char *tag, UCHAR *pVal, int bSet);
void accPrivateProfInt(const char *tag, int *pVal, int bSet);
void accPrivateProfBool(const char *tag, BOOL *pVal, int bSet);
void accPrivateProfFloat(const char *tag, float *pVal, int bSet);
void accPrivateProfDouble(const char *tag, double *pVal, int bSet);

void accPrivateProfStr(const char *tag, CString &csVal, int bSet); // note that for "get" the passed param csVal is the default

void accPrivateProfCptr(const char *tag, char **pptr, int bSave);
void accPrivateProfCbuf(const char *tag, char *buf, int maxLen, int bSave);
void accPrivateProfChar(const char *tag, char *buf, int maxLen, int bSave);

void accPrivateProfLONGLONG(const char *tag, LONGLONG *pVal, int bSet);
void accPrivateProfULONGLONG(const char *tag, ULONGLONG *pVal, int bSet);


#endif // _PRIVATE_PROFILE_STUFF_HH_H_H_H_