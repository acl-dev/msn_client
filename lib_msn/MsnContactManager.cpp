#include "stdafx.h"
#include "Util.h"
#include "MsnContactTMP.h"
#include "MsnTicket.h"
#include "MsnGroup.h"
#include "MsnMemberShips.h"
#include "MsnContacts.h"
#include "MsnAddressBookAb.h"
#include ".\MsnContactManager.h"

CMsnContactManager::CMsnContactManager()
{
	STRING_SET_MEPTY(cache_key_);
	STRING_SET_MEPTY(preferred_hostname_);
	STRING_SET_MEPTY(session_id_);

	groups_ = NULL;
	memberShips_ = NULL;
	contacts_ = NULL;
	addressAb_ = NULL;

	debug_fpout_ = acl_vstream_fopen("contact.txt",
		O_CREAT | O_TRUNC | O_WRONLY, 0700, 4096);
	if (debug_fpout_ == NULL)
		logger_error("fopen contact.txt error(%s)", acl_last_serror());
}

CMsnContactManager::~CMsnContactManager(void)
{
	STRING_SAFE_FREE(cache_key_);
	STRING_SAFE_FREE(preferred_hostname_);
	STRING_SAFE_FREE(session_id_);

	if (groups_)
		delete groups_;
	if (memberShips_)
		delete memberShips_;
	if (contacts_)
		delete contacts_;
	if (addressAb_)
		delete addressAb_;

	if (debug_fpout_ == NULL)
		acl_vstream_fclose(debug_fpout_);
}

void CMsnContactManager::logger_format(const char* fmt, ...)
{
	if (debug_fpout_ == NULL)
		return;

	va_list ap;
	va_start(ap, fmt);
	acl_vstream_vfprintf(debug_fpout_, fmt, ap);
	va_end(ap);
}

bool CMsnContactManager::GetMessage(const CMsnTicket& ticket)
{
	acl::http_client client;

	acl::string addr(MSN_CONTACT_SERVER);

	// ���� MSN SSO ������(ssl ��ʽ)
	addr << ":443";
	if (client.open(addr.c_str()) == false)
		return (false);

	// �����ϵ���б�
	if (GetContacts(ticket, client) == false)
		return (false);

	// �ر�����
	client.get_stream().close();

	// �ٴδ�����
	addr << ":443";
	if (client.open(addr.c_str(), true) == false)
		return (false);

	// ��õ�ַ���б�
	if (GetAddresses(ticket, client) == false)
		return (false);

	// �����û��б�
	CreateUsers();
	return (true);
}

//////////////////////////////////////////////////////////////////////////

bool CMsnContactManager::GetContacts(const CMsnTicket& ticket,
	acl::http_client& client)
{
	// ����������
	acl::string request_body;
	if (BuildGetContactRequest(ticket, request_body) == false)
		return (false);

	//////////////////////////////////////////////////////////////////////////
	// �� MSN CONTACT ������������������

	// ���� HTTP ����ͷ
	acl::http_header header;
	header.set_method(acl::HTTP_METHOD_POST);
	header.set_url(MSN_GET_CONTACT_POST_URL);
	header.set_host(MSN_CONTACT_SERVER);
	header.set_content_length(request_body.length());
	header.set_content_type("text/xml; charset=utf-8");
	header.set_keep_alive(false);
	header.add_entry("SOAPAction", MSN_GET_CONTACT_SOAP_ACTION);
	header.add_entry("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)");
	header.add_entry("Accept", "*/*");
	header.add_entry("Cache-Control", "no-cache");
	// ����ACL_CPP��ѹ�����֧��HTTP�����е�GZIP��ʽ
	header.accept_gzip(true);

	// ���� HTTP ����ͷ

	acl::string header_buf;
	header.build_request(header_buf);

	// ����־
	logger_format("send: %s\r\n", header_buf.c_str());

	if (client.get_ostream().write(header_buf) == -1)
	{
		logger_error("write http header error");
		return (false);
	}

	// ���� HTTP ������

	// ��¼������
	logger_format("send: %s\r\n", request_body.c_str());

	if (client.get_ostream().write(request_body) == -1)
	{
		logger_error("write http body error");
		return (false);
	}

	//////////////////////////////////////////////////////////////////////////
	// �� MSN CONTACT ��������ȡ��Ӧ����

	if (client.read_head() == false)
	{
		logger_error("read http respond head error");
		return (false);
	}

	const HTTP_HDR_RES* hdr_res = client.get_respond_head(NULL);
	acl_assert(hdr_res);

	// ��¼��Ӧͷ
	if (debug_fpout_)
		http_hdr_fprint(debug_fpout_, &hdr_res->hdr, "GetContacts read hdr: ");

	// ���û�������壬����Ϊʧ��
	if (hdr_res->hdr.content_length == 0
		|| (hdr_res->hdr.content_length == -1
		&& !hdr_res->hdr.chunked
		&& hdr_res->reply_status > 300
		&& hdr_res->reply_status < 400))
	{
		logger_error("http respond no body");
		http_hdr_print(&hdr_res->hdr, "error");
		return (false);
	}

	/* ���� HTTP ��Ӧ�� */

	ACL_XML* body = acl_xml_alloc();
	int   ret;
	acl::string plain;

	acl::ofstream fp;
	fp.open_write("zip_list.text");

	int  len = 0;
	while(true)
	{
		// �� HTTP ��Ӧ������
		ret = client.read_body(plain);
		if (ret < 0)
			break;
		else if (client.body_finish())
			break;
		fp.write(plain);
		acl_xml_update(body, plain.c_str());
	}

	//acl_xml_dump(body, ACL_VSTREAM_OUT);
	// ������ϵ���б�
	if (ParseContacts(body) == false)
	{
		acl_xml_free(body);
		return (false);
	}

	acl_xml_free(body);
	return (true);
}

bool CMsnContactManager::BuildGetContactRequest(const CMsnTicket& ticket, acl::string& out)
{
	const TICKET* tt = ticket.GetTicket("contacts.msn.com");
	if (tt == NULL)
	{
		logger_error("contacts.msn.com no found");
		return (false);
	}
	else if (tt->ticket == NULL)
	{
		logger_error("contactts.msn.com's ticket null");
		return (false);
	}

	acl::string buf;
	buf.format("t=%s&amp;p=", tt->ticket, tt->p ? tt->p : "");
	out.format(MSN_GET_CONTACT_TEMPLATE, "Initial", buf.c_str(), "");
	return (true);
}

bool CMsnContactManager::ParseContacts(ACL_XML* body)
{
	ACL_XML xml;

	//////////////////////////////////////////////////////////////////////////
	/// ���� soap:Header ����

	const char* tag_ServiceHeader = "soap:Header/ServiceHeader";
	ACL_ARRAY* a_ServiceHeader = acl_xml_getElementsByTags(body, tag_ServiceHeader);
	if (a_ServiceHeader == NULL)
	{
		logger_error("tag(%s) not found", tag_ServiceHeader);
		return false;
	}
	ACL_XML_NODE* ServiceHeader = (ACL_XML_NODE*) a_ServiceHeader->items[0];
	acl_xml_foreach_init(&xml, ServiceHeader);
	ParseContactHeader(&xml);
	acl_xml_free_array(a_ServiceHeader);

	//////////////////////////////////////////////////////////////////////////
	/// ���� soap:Body �е� Memberships/Membership ����

	const char* tag_members = "soap:Body/FindMembershipResponse"
		"/FindMembershipResult/Services/Service/Memberships/Membership";
	ACL_ARRAY* a_Membership = acl_xml_getElementsByTags(body, tag_members);
	if (a_Membership == NULL)
	{
		logger_error("tag(%s) not found", tag_members);
		return false;
	}

	// �������е� Membership XML �����󣬲�����һ�� Membership ���󼯺�
	memberShips_ = CMsnMemberShips::Create(a_Membership);
	acl_xml_free_array(a_Membership);

	// ���ԣ�����������
	memberShips_->Out();
	return true;
}

void CMsnContactManager::ParseContactHeader(ACL_XML* xml)
{
	cache_key_ = GetFirstText(xml, "CacheKey");
	preferred_hostname_ = GetFirstText(xml, "PreferredHostName");
	session_id_ = GetFirstText(xml, "SessionId");
}

const std::list<CMembership*>& CMsnContactManager::GetMemberships(void) const
{
	return (memberShips_->GetMemberShips());
}

// ������ϵ���б��еĳ�Ա������״̬
Member* CMsnContactManager::SetUserStatus(const char* passportName,
	const char* status, const char* displayName)
{
	acl_assert(passportName && status && displayName);

	std::map<acl::string, Member*>::iterator it = users_.find(passportName);
	if (it == users_.end())
		return (NULL);

	Member* member = it->second;
	if (strcasecmp(status, "NLN") == 0)
		member->online_ = member_s_online;
	else if (strcasecmp(status, "IDL") == 0)
		member->online_ = member_s_idle;
	else if (strcasecmp(status, "BSY") == 0)
		member->online_ = member_s_busy;
	else if (strcasecmp(status, "AWY"))
		member->online_ = member_s_away;
	else
		logger_warn("unkown status: %s", status);

	acl::string buf;

	// �ַ���ת���� GB18030
	bool ret = conv_.convert("UTF-8", "GB18030", displayName,
		strlen(displayName), &buf);
	if (ret == false)
	{
		logger_warn("convert from utf-8 to GB18030 error");
		return (member);
	}

	if (!STRING_IS_EMPTY(member->DisplayName))
		acl_myfree(member->DisplayName);
	member->DisplayName = acl_mystrdup(buf.c_str());

	return (member);
}

//////////////////////////////////////////////////////////////////////////

bool CMsnContactManager::GetAddresses(const CMsnTicket& ticket,
	acl::http_client& client)
{
	acl::string request_body;
	if (BuildGetAddressRequest(ticket, request_body) == false)
		return (false);

	///////////////////////////////////////////////////////////////////////
	// �� MSN CONTACT ������������������

	// ���� HTTP ����ͷ

	acl::http_header header;
	header.set_method(acl::HTTP_METHOD_POST);
	header.set_url(MSN_ADDRESS_BOOK_POST_URL);
	header.set_content_type("text/xml; charset=utf-8");
	header.set_host(MSN_CONTACT_SERVER);
	header.set_content_length(request_body.length());
	header.set_keep_alive(false);
	header.add_entry("SOAPAction", MSN_GET_ADDRESS_SOAP_ACTION);
	header.add_entry("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)");
	header.add_entry("Accept", "*/*");
	header.add_entry("Cache-Control", "no-cache");
	header.accept_gzip(true);

	// ���� HTTP ����ͷ

	acl::string header_buf;
	header.build_request(header_buf);

	// ��¼����ͷ
	logger_format("send header: %s\r\n", header_buf.c_str());

	if (client.get_ostream().write(header_buf) == -1)
	{
		logger_error("write http header error");
		return (false);
	}

	// ���� HTTP ������

	// ��¼������
	logger_format("send body: %s\r\n", request_body.c_str());

	if (client.get_ostream().write(request_body) == -1)
	{
		logger_error("write http body error");
		return (false);
	}

	///////////////////////////////////////////////////////////////////////
	// �� MSN CONTACT ��������ȡ��Ӧ����

	if (client.read_head() == false)
	{
		logger_error("read http respond head error");
		return (false);
	}

	const HTTP_HDR_RES* hdr_res = client.get_respond_head(NULL);

	if (debug_fpout_)
		http_hdr_fprint(debug_fpout_, &hdr_res->hdr, "GetAddresses read header: ");

	if (hdr_res->hdr.content_length == 0
		|| (hdr_res->hdr.content_length == -1
		&& !hdr_res->hdr.chunked
		&& hdr_res->reply_status > 300
		&& hdr_res->reply_status < 400))
	{
		logger_error("http respond no body");
		http_hdr_print(&hdr_res->hdr, "error");
		return (false);
	}

	/* ���� HTTP ��Ӧ�� */

	acl::string buf;

	ACL_SLICE_POOL* slice = acl_slice_pool_create(16, 50,
					ACL_SLICE_FLAG_GC2 |
					ACL_SLICE_FLAG_RTGC_OFF |
					ACL_SLICE_FLAG_LP64_ALIGN);
	ACL_XML* body = acl_xml_alloc1(slice);
	int   ret;
	while(true)
	{
		ret = client.read_body(buf);
		if (ret < 0)
			break;
		else if (client.body_finish())
			break;
		acl_xml_update(body, buf.c_str());
		logger_format("read body: %s\r\n", buf.c_str());
	}

	//acl_xml_dump(body, ACL_VSTREAM_OUT);
	ParseAddresses(body);

	///////////////////////////////////////////////////////////////////////

	double j = ACL_METER_TIME("---begin---");
	//acl_xml_free(body);
	acl_slice_pool_destroy(slice);
	double k = ACL_METER_TIME("---end---");
	return (true);
}

bool CMsnContactManager::BuildGetAddressRequest(const CMsnTicket& ticket,
	acl::string& out)
{
	const TICKET* tt = ticket.GetTicket("contacts.msn.com");
	if (tt == NULL)
	{
		logger_error("contacts.msn.com no found");
		return (false);
	}
	else if (tt->ticket == NULL)
	{
		logger_error("contactts.msn.com's ticket null");
		return (false);
	}

	acl::string buf, cache_key;
	if (cache_key_)
		cache_key << "<CacheKey>" << cache_key_ << "</CacheKey>";
	buf.format("t=%s&amp;p=", tt->ticket, tt->p ? tt->p : "");
	out.format(MSN_GET_ADDRESS_TEMPLATE, "Initial",
		cache_key.empty() ? "" : cache_key.c_str(), buf.c_str(), "");
	return (true);
}

bool CMsnContactManager::ParseAddresses(ACL_XML* body)
{
	///////////////////////////////////////////////////////////////////////
	// ���� soap:Body/ABFindAllResponse/ABFindAllResult
	const char* tag_ABFindAllResult =
		"soap:Body/ABFindAllResponse/ABFindAllResult";
	ACL_ARRAY* a_ABFindAllResult = acl_xml_getElementsByTags(body,
		tag_ABFindAllResult);
	if (a_ABFindAllResult == NULL)
	{
		logger("tag(%s) not found", tag_ABFindAllResult);
		return (true);
	}

	ACL_XML_NODE* ABFindAllResult = (ACL_XML_NODE*)
		acl_array_index(a_ABFindAllResult, 0);

	ACL_XML xml;
	acl_xml_foreach_init(&xml, ABFindAllResult);

	// �������е���

	ACL_ARRAY* a_group = acl_xml_getElementsByTags(&xml, "groups/Group");
	if (a_group != NULL)
	{
		// �����鼯�϶���
		groups_ = CMsnGroups::Create(a_group);
		acl_xml_free_array(a_group);
		groups_->Out();
	}

	// �������е���ϵ��

	ACL_ARRAY* a_contacts = acl_xml_getElementsByTags(&xml,
		"contacts/Contact");
	if (a_contacts != NULL)
	{
		// ������ϵ�˼��϶���
		contacts_ = CMsnContacts::Create(a_contacts, groups_);
		acl_xml_free_array(a_contacts);
		contacts_->Out();
	}

	ACL_ARRAY* a_addressAb = acl_xml_getElementsByTags(&xml, "ab");
	if (a_addressAb != NULL)
	{
		ACL_XML_NODE* node = (ACL_XML_NODE* )
			acl_array_index(a_addressAb, 0);
		acl_xml_foreach_init(&xml, node);
		addressAb_ = NEW CMsnAddressBookAb(&xml);
		acl_xml_free_array(a_addressAb);
	}

	acl_xml_free_array(a_ABFindAllResult);
	return (true);
}

Member* CMsnContactManager::FindUser(const char* passport)
{
	std::map<acl::string, Member*>::iterator it = users_.find(passport);
	if (it == users_.end())
		return (NULL);
	return (it->second);
}

void CMsnContactManager::AddUsers(const CMembership* memberShip)
{
	const std::list<Member*>& members = memberShip->GetMembers();
	std::list<Member*>::const_iterator cit = members.begin();
	for (; cit != members.end(); cit++)
	{
		const char* passport = (*cit)->PassportName;
		if (STRING_IS_EMPTY(passport))
			continue;
		Member* member = FindUser(passport);
		if (member != NULL)
			continue;

		// �����ʾ��Ϊ�գ������� Passport �ʺŴ���
		if (STRING_IS_EMPTY((*cit)->DisplayName))
			(*cit)->DisplayName = acl_mystrdup(passport);
		users_[passport] = (*cit);
	}
}

void CMsnContactManager::CreateUsers()
{
	// �������г�Ա�б������û��б�

	const std::list<CMembership*>& memberShips =
		memberShips_->GetMemberShips();
	std::list<CMembership*>::const_iterator cit1 = memberShips.begin();
	for (; cit1 != memberShips.end(); cit1++)
	{
		const char* role = (*cit1)->GetMemberRole();
		if (STRING_IS_EMPTY(role))
			continue;
		if (strcmp(role, "Allow") == 0)
			AddUsers(*cit1);
		else if (strcmp(role, "Block") == 0)
			AddUsers(*cit1);
		else if (strcmp(role, "Reverse") == 0)
			AddUsers(*cit1);
	}

	// ����������ϵ���б������û��б����Ϣ

	const std::list<CMsnContact*>& contacts = contacts_->GetContacts();
	std::list<CMsnContact*>::const_iterator cit2 = contacts.begin();
	for (; cit2 != contacts.end(); cit2++)
	{
		// ��Ҫ�Լ��
		if (STRING_IS_EMPTY((*cit2)->passportName))
			continue;
		if (STRING_IS_EMPTY((*cit2)->contactType))
			continue;
		if (strcasecmp((*cit2)->contactType, "Regular") != 0)
			continue;
		Member* member = FindUser((*cit2)->passportName);
		if (member == NULL)
			continue;

		// ���øó�Ա����ϵ�˶���
		member->SetContact(*cit2);
	}
}

void CMsnContactManager::PrintUsers() const
{
	printf("-------------print all users--------------\r\n");

	std::map<acl::string, Member*>::const_iterator cit = users_.begin();
	for (; cit != users_.end(); cit++)
	{
		printf(">> user: %s, name: %s\r\n", cit->second->PassportName,
			cit->second->DisplayName);
	}

	// ��ӡ����Ϣ�Լ������ڸ������ϵ�˵�������Ϣ
	if (groups_)
		groups_->Out();
}