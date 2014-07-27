#pragma once
#include "lib_msn.h"

//////////////////////////////////////////////////////////////////////////
// msnclient �Ľӿ�

class LIB_MSN_API IMsnClient
{
public:
	IMsnClient() {}
	virtual ~IMsnClient() {}

	/**
	* ����ɹ�ʱ�Ļص�����
	*/
	virtual void OnLoginOk() {}

	/**
	* ����ʧ��ʱ�Ļص�����
	*/
	virtual void OnLoginErr() {}
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

class CMsnDSClient;
class CMsnNSClient;
class CMsnTicket;
class CMsnStatus;
class CMsnService;

// MSN �ͻ��˿�
class LIB_MSN_API CMsnClient
{
public:
	/**
	* ���캯��
	* @param callback {IMsnClient*} MSN Э�鴦������еĻص�����
	* @param account {const char*} �����ʺ�
	* @param passwd {const char*} ����
	* @param timeout {int} ��ʱʱ��ֵ
	*/
	CMsnClient(IMsnClient* callback, const char* account,
		const char* passwd, int timeout);
	~CMsnClient(void);

	/**
	* ��ʼ MSN �������
	* @param domain {const char*} ����ķ���������
	* @param port {unsigned short} �������˿�
	*
	*/
	void Login(const char* domain, unsigned short port);

	void OnDSOk(const acl::string& ns_addr);
	void OnDSErr();

	void OnNSUsrSSO();
	void OnNSErr();
	void OnNSClose();
	int  Sid();

	IMsnClient& GetCallback() const
	{
		return (*callback_);
	}

	// ����첽���̵ķ�����
	CMsnService& GetService()
	{
		return (*msn_service_);
	}

	CMsnNSClient* GetNSClient()
	{
		return (ns_client_);
	}

	/**
	* SSO ����ɹ���ʧ�ܵĻص�����
	* @param ticket ���ʧ�ܣ���ò���Ϊ��
	*/
	void OnSSOFinish(CMsnTicket* ticket);
protected:
private:
	IMsnClient* callback_;
	acl::string account_;
	acl::string passwd_;
	CMsnDSClient* ds_client_;
	CMsnNSClient* ns_client_;
	CMsnTicket* ticket_;
	acl::string ns_addr_;
	int   timeout_;
	int   sid_;

	CMsnService* msn_service_;
};
