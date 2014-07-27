#pragma once
#include "string.hpp"
#include "charset_conv.hpp"
#include "MsnUserList.h"
#include "lib_msn.h"

class CMsnTicket;
struct ACL_XML;
acl::http_client;
class CSSLClient;
class CMsnGroups;
class CMembership;
class CMsnMemberShips;
class CMsnContacts;
class CMsnAddressBookAb;
class Member;

struct ACL_VSTREAM;

class LIB_MSN_API CMsnContactManager
{
public:
	CMsnContactManager(void);
	~CMsnContactManager(void);

	// ������е���ϵ�˼���ַ��
	bool GetMessage(const CMsnTicket& ticket);

	const std::list<CMembership*>& GetMemberships(void) const;

	Member* SetUserStatus(const char* passportName, const char* status,
		const char* displayName);

	std::map<acl::string, Member*>& GetUsers(void)
	{
		return (users_);
	}

	// ���Խӿڣ�����û��б�
	void PrintUsers() const;
private:
	acl::charset_conv conv_;
	char* cache_key_;
	char* preferred_hostname_;
	char* session_id_;

	// ���б�
	CMsnGroups* groups_;
	// ��Ա�б�
	CMsnMemberShips* memberShips_;
	// ��ϵ���б�
	CMsnContacts* contacts_;
	// ��ַ���б�
	CMsnAddressBookAb* addressAb_;

	bool GetContacts(const CMsnTicket& ticket, acl::http_client& client);
	bool BuildGetContactRequest(const CMsnTicket& ticket, acl::string& out);
	bool ParseContacts(ACL_XML* body);
	void ParseContactHeader(ACL_XML* xml);
	void OutMembers(void);

	bool GetAddresses(const CMsnTicket& ticket, acl::http_client& client);
	bool BuildGetAddressRequest(const CMsnTicket& ticket, acl::string& out);
	bool ParseAddresses(ACL_XML* body);
	void OutGroups(void);
	void OutContacts(void);

	// ��ϵ���б��� memberShips_ �� contacts_ ��϶���
	std::map<acl::string, Member*> users_;
	// ���� MSN �û��б�
	void CreateUsers(void);
	Member* FindUser(const char* passport);
	void AddUsers(const CMembership* memberShip);

	ACL_VSTREAM* debug_fpout_;
	void logger_format(const char* fmt, ...);
};
