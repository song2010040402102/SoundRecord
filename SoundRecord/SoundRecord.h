// SoundRecord.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CSoundRecordApp:
// �йش����ʵ�֣������ SoundRecord.cpp
//

class CSoundRecordApp : public CWinApp
{
public:
	CSoundRecordApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSoundRecordApp theApp;
