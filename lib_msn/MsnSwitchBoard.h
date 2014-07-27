#pragma once
#include <time.h>
#include "string.hpp"
#include "mime.hpp"
#include "charset_conv.hpp"
#include "MsnChat.h"
#include "lib_msn.h"

class CMsnNSClient;

/**
* ������Ϣ����
*/
class LIB_MSN_API CMsnMsg
{
public:
	CMsnMsg(const char* data, size_t dlen)
	{
		data_.copy(data, dlen);
		stamp_ = time(NULL);
	}

	~CMsnMsg()
	{

	}

	const acl::string& GetData() const
	{
		return (data_);
	}

	time_t GetStamp() const
	{
		return (stamp_);
	}
private:
	time_t stamp_;
	acl::string data_;
};

//////////////////////////////////////////////////////////////////////////

// �ö���������MSN����Ϣ�������Խӣ����յ������ڿͻ�����Ϣʱ��
// �������ö��󣬽������Ϊһ���������̣�����һ���������߳���
// ���У��������߳�֮�����IPCͨ�ŷ�ʽ
// ���е� CMsnSwitchBoard ���󶼱��������ĳһ�� CMsnNSClient ����
// ֻ�� CMsnNSClient ���󱣳ִ��ʱ CMsnSwitchBoard ������ܴ��

class LIB_MSN_API CMsnSwitchBoard : public CMsnChat
{
public:
	// ���캯��
	CMsnSwitchBoard(CMsnNSClient* ns_client, const char* peer_user);

	// ��������
	~CMsnSwitchBoard(void);

	// �Ѿ����ӳɹ�����ֱ�ӷ���
	void SendMsg(const char* data);
	void AppendMsg(const char* data);
	void SendMsg(CMsnMsg* msg);

	// ���ûỰ��֤�� KEY
	void SetAuthKey(const char* auth_key);

	/**
	* ���ûỰ ID
	* @param sid {const char*}
	*/
	void SetSid(const char* sid);

	/**
	* �Ƿ�����Ϊ�����뷽ʽ
	* @param yesno {bool} ���Ϊ true ������Ϊ�������뷽ʽ��
	*  ����Ϊ�������뷽ʽ
	* ע����������ñ������������뷽ʽ����Ĭ��Ϊ�������뷽ʽ
	*/
	void SetInvited(bool yesno = true);

	/**
	* �ж��Ƿ����뷽ʽ
	* @return {bool}
	*/
	bool IsInvited(void) const;

	bool IsConnected(void) const
	{
		return (is_connected_);
	}

	void SetXfrId(int xfr_id)
	{
		xfr_id_ = xfr_id;
	}

	int GetXfrId() const
	{
		return (xfr_id_);
	}

	const acl::string& GetPeerUser() const
	{
		return (peer_user_);
	}

	void Quit();

	// ����������
	void AddUser(const acl::string& user_in,
		const acl::string& display_name);
protected:
	virtual void OnOpen();
	virtual void OnRead(char* data, int len);
	virtual void OnSend();
	virtual void OnTimeout();
	virtual void OnClose();
private:
	acl::string peer_user_;
	acl::string sid_;
	acl::string auth_key_;
	acl::string server_addr_;
	CMsnNSClient* ns_client_;
	bool is_invited_;
	int id_;
	int xfr_id_;
	int curr_seq_;
	bool ready_;
	bool is_connected_;
	bool is_quiting_;

	int status_;
#define  SB_STATUS_CMD	0
#define  SB_STATUS_MSG  1

	int total_users_;
	int current_users_;

	acl::charset_conv conv_;
	acl::mime mime_;
	int  msg_length_;
	int  msg_read_;

	// ������Ϣ����
	std::list<CMsnMsg*> chat_msgs_;

	// ��ǰ����������б�
	std::map<acl::string, acl::string> chat_users_;

	// ��ʼ�������
	void Start();

	void OnCmd(char* data, int dlen);
	void OnCmdANS(int argc, char** argv, const char* src);
	void OnCmdIRO(int argc, char** argv, const char* src);
	void OnCmdACK(int argc, char** argv, const char* src);
	void OnCmdNAK(int argc, char** argv, const char* src);
	void OnCmdUSR(int argc, char** argv, const char* src);
	void OnCmdMSG(int argc, char** argv, const char* src);
	void OnCmdUBM(int argc, char** argv, const char* src);
	void OnCmdJOI(int argc, char** argv, const char* src);
	void OnCmdBYE(int argc, char** argv, const char* src);
	void OnCmdOUT(int argc, char** argv, const char* src);

	void OnMsgData(char* data, int len);
	void OnMsgControl(int argc, const char** argv);
	void OnMsgText(int argc, const char** argv, const char* data, int len);
};
