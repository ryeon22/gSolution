#pragma once

#ifndef __AFXWIN_H__
#endif

#include "resource.h"		

class CgPrjApp : public CWinApp
{
public:
	CgPrjApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CgPrjApp theApp;