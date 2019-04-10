//
// AppSpt.cpp
//
#include "Stdafx.h"
#include "AppSpt.h"

#define VMajor 1
#define VMinor 0
#define VUpdate 1

#ifdef _GUI 
#define _NameSuffix "(GUI)"
#endif 

#ifdef _TCM
#define _NameSuffix "(TCM)"
#endif 

#define ProductName "MB Dynamics Sample Eagle App"
#define ProductShortName "Sample_Eagle_App"



LPCTSTR AppSpt_getProductName(void)
{
	static CString aa;
	aa = ProductName;
	aa += " ";
	aa += _NameSuffix;
	return aa;
}

LPCTSTR AppSpt_getProductShortName(void)
{
	static CString aa;
	aa = ProductShortName;
	aa += " ";
	aa += _NameSuffix;
	return aa;
}

void    AppSpt_getProductVersion(int *pmaj, int *pmin, int *pup)
{
	*pmaj = VMinor;
	*pmin = VMinor;
	*pup  = VUpdate;
}

LPCTSTR AppSpt_getMainTitle(void)
{
	static CString aa;
	CString space = " ";
	aa = AppSpt_getProductName();
	CString ver;
	ver.Format("V%d.%d.%d", VMajor, VMinor, VUpdate);

	aa += space + ver;

	return aa;
}

LPCTSTR AppSpt_getHelpAboutDisplayString(void)
{
	static CString aa;
	CString space = " ";
	aa = AppSpt_getProductName();
	CString ver;
	ver.Format("Version %d.%d.%d", VMajor, VMinor, VUpdate);

	aa += space + ver;
	
	CString b;
	b.Format("(Build %s)", __DATE__);
	aa +=  space + b;

	return aa;
}

// EOF