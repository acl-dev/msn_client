// MsnDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CMsnDemoApp:
// �йش����ʵ�֣������ MsnDemo.cpp
//

class CMsnDemoApp : public CWinApp
{
public:
	CMsnDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMsnDemoApp theApp;
