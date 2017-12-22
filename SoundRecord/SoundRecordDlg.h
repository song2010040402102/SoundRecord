// SoundRecordDlg.h : 头文件
//

#pragma once
#include ".\soundrecordengine.h"

// CSoundRecordDlg 对话框
class CSoundRecordDlg : public CDialog
{
// 构造
public:
	CSoundRecordDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SOUNDRECORD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CSoundRecordEngine m_clsSRE; //声音录制引擎
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();	
	DECLARE_MESSAGE_MAP()
private:
	//录音完成消息回调函数
	afx_msg LRESULT OnRecordComplete(WPARAM wParam, LPARAM lParam);
public:	
	afx_msg void OnBnClickedButtonRecordStart();
	afx_msg void OnBnClickedButtonRecordStop();
	afx_msg void OnBnClickedButtonRecordSave();
	afx_msg void OnBnClickedButtonRecordPlay();
};
