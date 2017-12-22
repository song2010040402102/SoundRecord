// SoundRecordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SoundRecord.h"
#include "SoundRecordDlg.h"
#include ".\soundrecorddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSoundRecordDlg 对话框



CSoundRecordDlg::CSoundRecordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundRecordDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSoundRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSoundRecordDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_RECORD_START, OnBnClickedButtonRecordStart)
	ON_BN_CLICKED(IDC_BUTTON_RECORD_STOP, OnBnClickedButtonRecordStop)
	ON_BN_CLICKED(IDC_BUTTON_RECORD_SAVE, OnBnClickedButtonRecordSave)
	ON_BN_CLICKED(IDC_BUTTON_RECORD_PLAY, OnBnClickedButtonRecordPlay)
	ON_MESSAGE(WM_RECORD_COMPLETE, OnRecordComplete)
END_MESSAGE_MAP()


// CSoundRecordDlg 消息处理程序

BOOL CSoundRecordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CSoundRecordDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSoundRecordDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CSoundRecordDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSoundRecordDlg::OnBnClickedButtonRecordStart()
{
	// TODO: 在此添加控件通知处理程序代码
	m_clsSRE.SetRecordListenWnd(this->GetSafeHwnd());
	m_clsSRE.StartRecord(5); //启动5秒的录音
}

void CSoundRecordDlg::OnBnClickedButtonRecordStop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_clsSRE.StopRecord();
}

void CSoundRecordDlg::OnBnClickedButtonRecordSave()
{
	// TODO: 在此添加控件通知处理程序代码
	m_clsSRE.SaveRecord("1.amr", SF_AMR);
}

void CSoundRecordDlg::OnBnClickedButtonRecordPlay()
{
	// TODO: 在此添加控件通知处理程序代码
}

LRESULT CSoundRecordDlg::OnRecordComplete(WPARAM wParam, LPARAM lParam)
{
	m_clsSRE.StopRecord();
	return 0;	
}