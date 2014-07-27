#pragma once

class CMsnService : public acl::ipc_service
{
public:
	CMsnService(void);
	~CMsnService(void);

	// ���� SSO �������
	void start_sso(CMsnClient* client, const char* account,
		const char* passwd);

	// �����ϵ�˹���
	void start_get_contact(CMsnNSClient* ns_client, const CMsnTicket& ticket);
private:
	void win32_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
