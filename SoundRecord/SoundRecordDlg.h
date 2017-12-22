// SoundRecordDlg.h : ͷ�ļ�
//

#pragma once
#include ".\soundrecordengine.h"

// CSoundRecordDlg �Ի���
class CSoundRecordDlg : public CDialog
{
// ����
public:
	CSoundRecordDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SOUNDRECORD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CSoundRecordEngine m_clsSRE; //����¼������
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();	
	DECLARE_MESSAGE_MAP()
private:
	//¼�������Ϣ�ص�����
	afx_msg LRESULT OnRecordComplete(WPARAM wParam, LPARAM lParam);
public:	
	afx_msg void OnBnClickedButtonRecordStart();
	afx_msg void OnBnClickedButtonRecordStop();
	afx_msg void OnBnClickedButtonRecordSave();
	afx_msg void OnBnClickedButtonRecordPlay();
};
