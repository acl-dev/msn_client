// MsnDemoDlg.h : ͷ�ļ�
//

#pragma once

#include "MsnClient.h"
#include "lib_msn.h"

// CMsnDemoDlg �Ի���
class CMsnDemoDlg : public CDialog, public IMsnClient
{
// ����
public:
	CMsnDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

	~CMsnDemoDlg();				// ��׼��������

// �Ի�������
	enum { IDD = IDD_MSNDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLogin();

private:
	CMsnClient* msn_client_;

	bool GetLoginInfo(CString& account, CString& passwd);
	bool GetChatInfo(CString& toUser, CString& data);
public:
	afx_msg void OnBnClickedSend();
};
