//
// AppSpt.h
//
#pragma once

LPCTSTR AppSpt_getProductName(void);
LPCTSTR AppSpt_getProductShortName(void);
void    AppSpt_getProductVersion(int *pmaj, int *pmin, int *pup);
LPCTSTR AppSpt_getHelpAboutDisplayString(void);
LPCTSTR AppSpt_getMainTitle(void);

// EOF