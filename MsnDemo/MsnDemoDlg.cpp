// MsnDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MsnDemo.h"
#include "MsnDemoDlg.h"
#include "MsnGlobal.h"
#include "MsnNSClient.h"
#include ".\msndemodlg.h"

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


// CMsnDemoDlg �Ի���



CMsnDemoDlg::CMsnDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsnDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	msn_client_ = NULL;
}

CMsnDemoDlg::~CMsnDemoDlg()
{
	if (msn_client_)
		delete msn_client_;
}

void CMsnDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMsnDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOGIN, OnBnClickedLogin)
	ON_BN_CLICKED(IDC_SEND, OnBnClickedSend)
END_MESSAGE_MAP()


// CMsnDemoDlg ��Ϣ�������

BOOL CMsnDemoDlg::OnInitDialog()
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

	//ShowWindow(SW_MAXIMIZE);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// ��ʼ�� AIO ���
	(void) get_aio_handle(true);

	// �� DOS �ն�
	AllocConsole();
	FILE* fp = freopen("CONOUT$","w+t",stdout);

	GetDlgItem(IDC_USERNAME)->SetWindowText("zhengshuxin@hotmail.com");
	GetDlgItem(IDC_PASSWD)->SetWindowText("zsxNihao");

	GetDlgItem(IDC_TO_USER)->SetWindowText("forward_day@hotmail.com");
	GetDlgItem(IDC_INPUT)->SetWindowText("hello, who're you!");

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CMsnDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMsnDemoDlg::OnPaint() 
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

bool CMsnDemoDlg::GetLoginInfo(CString& account, CString& passwd)
{
	GetDlgItem(IDC_USERNAME)->GetWindowText(account);
	if (account.GetLength() == 0) {
		MessageBox("�ʺ�Ϊ��!");
		return (false);
	}
	GetDlgItem(IDC_PASSWD)->GetWindowText(passwd);
	if (passwd.GetLength() == 0)
	{
		MessageBox("����Ϊ��!");
		return (false);
	}
	return (true);
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CMsnDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMsnDemoDlg::OnBnClickedLogin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString account, passwd;

	if (GetLoginInfo(account, passwd) == false)
		return;

	// ��ֹ�ظ�����
	GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);

	msn_client_ = new CMsnClient(this, account.GetString(), passwd.GetString(), 60);
	msn_client_->Login("messenger.hotmail.com", 1863);
}


bool CMsnDemoDlg::GetChatInfo(CString& toUser, CString& data)
{
	GetDlgItem(IDC_TO_USER)->GetWindowText(toUser);
	if (toUser.GetLength() == 0)
	{
		MessageBox("�������Ϊ��!");
		return (false);
	}
	GetDlgItem(IDC_INPUT)->GetWindowText(data);
	if (data.GetLength() == 0) {
		MessageBox("����Ϊ��!");
		return (false);
	}

	return (true);
}

void CMsnDemoDlg::OnBnClickedSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (msn_client_ == NULL)
	{
		MessageBox("���ȵ���!");
		return;
	}

	CString data, toUser;

	if (GetChatInfo(toUser, data) == false)
		return;
	GetDlgItem(IDC_INPUT)->SetWindowText("");
	CMsnNSClient* ns_client = msn_client_->GetNSClient();
	if (ns_client == NULL)
	{
		MessageBox("You're quit now!");
		return;
	}
	ns_client->SpeakTo(toUser.GetString(), data.GetString(), data.GetLength());
}
