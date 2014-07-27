#include "stdafx.h"
#include "lib_acl.h"
#include "MsnUserList.h"
#include "MsnContacts.h"
#include "Util.h"
#include ".\msnmemberships.h"

//////////////////////////////////////////////////////////////////////////

const char* StatusToString(MemberStatus status)
{
	static char* _unknown = "unknown";
	if (status > member_s_busy)
		return (_unknown);

	static char* _values[] =
	{
		"offline", "online", "idle", "away", "busy"
	};
	size_t n = (size_t) status;
	return (_values[n]);
}

//////////////////////////////////////////////////////////////////////////
//
//MemberCmp::MemberCmp()
//{
//
//}
//
//MemberCmp::~MemberCmp()
//{
//
//}
//
//bool MemberCmp::operator ()(const Member* a, const Member* b) const
//{
//	if (STRING_IS_EMPTY(a->PassportName))
//		return (false);
//	else if (STRING_IS_EMPTY(b->PassportName))
//		return (true);
//	int  ret = strcmp(a->PassportName, b->PassportName);
//	if (ret < 0)
//		return (true);
//	else
//		return (false);
//}

//////////////////////////////////////////////////////////////////////////

//   <Member xsi:type="PassportMember">
//    <MembershipId>64</MembershipId>
//    <Type>Passport</Type>
//    <State>Accepted</State>
//    <Deleted>false</Deleted>
//    <LastChanged>2010-10-11T20:26:42.533-07:00</LastChanged>
//    <JoinedDate>0001-01-01T00:00:00-08:00</JoinedDate>
//    <ExpirationDate>0001-01-01T00:00:00</ExpirationDate>
//    <Changes />
//    <PassportName>heshanlily@hotmail.com</PassportName>
//    <IsPassportNameHidden>false</IsPassportNameHidden>
//    <PassportId>0</PassportId>
//    <CID>7360734171765726922</CID>
//    <PassportChanges />
//    <LookedupByCID>false</LookedupByCID>
//   </Member>

Member::Member()
{
	DisplayName = NULL;
	PassportName = NULL;
	IsPassportNameHidden = false;
	PassportId = -1;
	CID = NULL;
	LookedupByCID = false;
	Email = NULL;
	Phone = NULL;
	MembershipId = -1;
	Type = NULL;
	State = NULL;
	Deleted = false;
	LastChanged = NULL;
	JoinedDate = NULL;
	ExpirationDate = NULL;

	op_ = MSN_LIST_NULL;
	authorized_ = false;
	online_ = member_s_offline;
	contact_ = NULL;
}

Member::~Member()
{
	STRING_SAFE_FREE(CID);
	STRING_SAFE_FREE(DisplayName);
	STRING_SAFE_FREE(ExpirationDate);
	STRING_SAFE_FREE(JoinedDate);
	STRING_SAFE_FREE(LastChanged);
	STRING_SAFE_FREE(PassportName);
	STRING_SAFE_FREE(State);
	STRING_SAFE_FREE(Type);
}

void Member::SetContact(CMsnContact* contact)
{
	contact_ = contact;

	// ������ʾ����

	if (STRING_IS_EMPTY(contact->displayName))
		return;
	if (!STRING_IS_EMPTY(DisplayName))
		acl_myfree(DisplayName);
	DisplayName = acl_mystrdup(contact->displayName);
}

void Member::Out() const
{
	printf(">> Member: DisplayName: %s, PassportName: %s, State: %s\r\n",
		DisplayName ? DisplayName : "unkown",
		PassportName ? PassportName : "unknown",
		State ? State : "unkown");
}

//////////////////////////////////////////////////////////////////////////

// <Membership>
//  <MemberRole>Allow</MemberRole>
//  <Members>
//   <Member xsi:type="PassportMember">
//    <MembershipId>64</MembershipId>
//    <Type>Passport</Type>
//    <State>Accepted</State>
//    <Deleted>false</Deleted>
//    <LastChanged>2010-10-11T20:26:42.533-07:00</LastChanged>
//    <JoinedDate>0001-01-01T00:00:00-08:00</JoinedDate>
//    <ExpirationDate>0001-01-01T00:00:00</ExpirationDate>
//    <Changes />
//    <PassportName>heshanlily@hotmail.com</PassportName>
//    <IsPassportNameHidden>false</IsPassportNameHidden>
//    <PassportId>0</PassportId>
//    <CID>7360734171765726922</CID>
//    <PassportChanges />
//    <LookedupByCID>false</LookedupByCID>
//   </Member>
//   ...
//  </Members>
// </Membership>

CMembership::CMembership(ACL_XML& xmlMembership, acl::charset_conv* conv)
{
	ACL_ITER iter;
	ACL_XML xmlMember;

	// ��ó�Ա��ɫ���ƣ�Allow/Block/Reverse/Pending
	MemberRole_ = GetFirstText(&xmlMembership, "MemberRole");
	// ����ɫ��ת����Ӧ�ı�ʶ��
	listId_ = msn_get_memberrole(MemberRole_);
	listOp_ = (MsnListOp) (1 << listId_);

	const char* tag = "Members/Member";
	ACL_ARRAY* a_members = acl_xml_getElementsByTags(&xmlMembership, tag);

	acl_foreach(iter, a_members)
	{
		ACL_XML_NODE* node = (ACL_XML_NODE*) iter.data;
		acl_xml_foreach_init(&xmlMember, node);
		AddMember(&xmlMember, conv, listOp_);
	}
	acl_xml_free_array(a_members);
}

CMembership::~CMembership(void)
{
	STRING_SAFE_FREE(MemberRole_);

	std::list<Member*>::iterator it = members_.begin();
	for (; it != members_.end(); it++)
	{
		Member* member = *it;
		delete member;
	}
}

void CMembership::AddMember(ACL_XML* xmlMember, acl::charset_conv* conv,
	MsnListOp listOp)
{
	Member* member;

	char* Type = GetFirstText(xmlMember, "Type");

	if (STRING_IS_EMPTY(Type))
		return;

	if (strcmp(Type, "Passport") == 0)
		member = GetPassportMember(xmlMember, conv);
	else if (strcmp(Type, "Email") == 0)
		member = GetEmailMember(xmlMember);
	else if (strcmp(Type, "Phone") == 0)
		member = GetPhoneMember(xmlMember);
	else
	{
		logger_warn("unknown Type: %s", Type);
		member = NULL;
	}

	if (member != NULL)
	{
		member->op_ = listOp;
		if ((member->op_ & MSN_LIST_PL_OP))
			member->authorized_ = true;
		else
			member->authorized_ = false;

		member->op_ = (MsnListOp) (member->op_ | MSN_LIST_FL_OP);

		// ��ӽ���ϵ�˼�����
		members_.push_back(member);
	}

	STRING_SAFE_FREE(Type);
}

void CMembership::SetMemberCommon(ACL_XML* xmlMember, Member* member)
{
	char* ptr = GetFirstText(xmlMember, "MembershipId");
	member->MembershipId = atoi(ptr);
	STRING_SAFE_FREE(ptr);

	member->Type =  GetFirstText(xmlMember, "Type");
	member->State = GetFirstText(xmlMember, "State");

	ptr = GetFirstText(xmlMember, "Deleted");
	if (strcasecmp(ptr, "true") == 0)
	{
		member->Deleted = true;
		STRING_SAFE_FREE(ptr);
	}
	else
		member->Deleted = false;

	member->LastChanged = GetFirstText(xmlMember, "LastChanged");
	member->JoinedDate = GetFirstText(xmlMember, "JoinedDate");
	member->ExpirationDate = GetFirstText(xmlMember, "ExpirationDate");
}

Member* CMembership::GetPassportMember(ACL_XML* xmlMember, acl::charset_conv* conv)
{
	// PassportName ����Ϊ��
	char* PassportName = GetFirstText(xmlMember, "PassportName");
	if (STRING_IS_EMPTY(PassportName))
		return (NULL);
	// �� PassportName ��Ч���򷵻ؿ�
	else if (MsnEmailIsValid(PassportName) == false)
	{
		STRING_SAFE_FREE(PassportName);
		return (NULL);
	}

	// �����ʾ����
	char* DisplayName = GetFirstText(xmlMember, "DisplayName");
	if (!STRING_IS_EMPTY(DisplayName) && conv != NULL)
	{
		acl::string buf;

		if (conv->convert("utf-8", "gb18030", DisplayName,
			strlen(DisplayName), &buf) == true)
		{
			STRING_SAFE_FREE(DisplayName);
			DisplayName = acl_mystrdup(buf.c_str());
		}
		else
			logger_error("Displayname: %p, %p, (%s)\n",
				var_emty_string, DisplayName, DisplayName);
	}

	char* ptr;
	Member* member = NEW Member;

	member->PassportName = PassportName;
	member->DisplayName = DisplayName;

	ptr = GetFirstText(xmlMember, "IsPassportNameHidden");
	if (strcasecmp(ptr, "true") == 0)
	{
		member->IsPassportNameHidden = true;
		STRING_SAFE_FREE(ptr);
	}
	else
		member->IsPassportNameHidden = false;

	ptr = GetFirstText(xmlMember, "PassportId");
	member->PassportId = atoi(ptr);
	STRING_SAFE_FREE(ptr);

	member->CID = GetFirstText(xmlMember, "CID");

	ptr = GetFirstText(xmlMember, "LookedupByCID");
	if (strcasecmp(ptr, "true") == 0)
	{
		member->LookedupByCID = true;
		STRING_SAFE_FREE(ptr);
	}
	else
		member->LookedupByCID = false;

	// ����ͨ����
	SetMemberCommon(xmlMember, member);
	return (member);
}

Member* CMembership::GetEmailMember(ACL_XML* xmlMember)
{
	// Email ����Ϊ��
	char* Email = GetFirstText(xmlMember, "Email");
	if (STRING_IS_EMPTY(Email))
		return (NULL);
	Member* member = NEW Member;
	member->Email = Email;

	// ����ͨ����
	SetMemberCommon(xmlMember, member);
	return (member);
}

Member* CMembership::GetPhoneMember(ACL_XML* xmlMember)
{
	// Phone ����Ϊ��
	char* Phone = GetFirstText(xmlMember, "Phone");
	if (STRING_IS_EMPTY(Phone))
		return (NULL);
	Member* member = NEW Member;
	member->Phone = Phone;

	// ����ͨ����
	SetMemberCommon(xmlMember, member);
	return (member);
}

void CMembership::Out(void) const
{
	printf(">> CMemberShip: role: %s\r\n", MemberRole_);
	std::list<Member*>::const_iterator it = members_.begin();
	for (; it != members_.end(); it++)
		(*it)->Out();
}

//////////////////////////////////////////////////////////////////////////

CMsnMemberShips::CMsnMemberShips(void)
{

}

CMsnMemberShips::~CMsnMemberShips(void)
{
	std::list<CMembership*>::iterator it = memberships_.begin();
	for (; it != memberships_.end(); it++)
		delete (*it);
}

// �������е� Membership ���󣬲�������ϵ�˳�Ա�б����
CMsnMemberShips* CMsnMemberShips::Create(ACL_ARRAY* memberShipArray)
{
	CMsnMemberShips* contacts = NEW CMsnMemberShips();
	acl::charset_conv conv;
	ACL_XML xml;

	// ���� XML �������е�ÿ����㣬ȡ����ϵ����Ϣ����������ϵ�˶��󼯺���

	ACL_ITER iter;
	acl_foreach(iter, memberShipArray)
	{
		ACL_XML_NODE* node = (ACL_XML_NODE*) iter.data;
		acl_xml_foreach_init(&xml, node);

		// ����һ���µĳ�Ա����ÿ�� Membership XML ��������һ������
		// �� XML �����Է������
		CMembership* memberShip = NEW CMembership(xml, &conv);
		contacts->memberships_.push_back(memberShip);
	}

	return (contacts);
}

std::list<CMembership*>& CMsnMemberShips::GetMemberShips(void)
{
	return (memberships_);
}

// �����Ա�б���Ϣ����Ļ
void CMsnMemberShips::Out(void) const
{
	std::list<CMembership*>::const_iterator it = memberships_.begin();
	for (; it != memberships_.end(); it++)
	{
		const CMembership* memberShip = *it;
		memberShip->Out();
	}
}