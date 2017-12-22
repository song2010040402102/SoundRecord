// SoundRecordDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SoundRecord.h"
#include "SoundRecordDlg.h"
#include ".\soundrecorddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CSoundRecordDlg �Ի���



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


// CSoundRecordDlg ��Ϣ�������

BOOL CSoundRecordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSoundRecordDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CSoundRecordDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSoundRecordDlg::OnBnClickedButtonRecordStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_clsSRE.SetRecordListenWnd(this->GetSafeHwnd());
	m_clsSRE.StartRecord(5); //����5���¼��
}

void CSoundRecordDlg::OnBnClickedButtonRecordStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_clsSRE.StopRecord();
}

void CSoundRecordDlg::OnBnClickedButtonRecordSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_clsSRE.SaveRecord("1.amr", SF_AMR);
}

void CSoundRecordDlg::OnBnClickedButtonRecordPlay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

LRESULT CSoundRecordDlg::OnRecordComplete(WPARAM wParam, LPARAM lParam)
{
	m_clsSRE.StopRecord();
	return 0;	
}