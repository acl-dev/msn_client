#pragma once
#include <list>
#include <set>
#include "string.hpp"
#include "MsnUserList.h"
#include "lib_msn.h"

// ��ϵ�˵�����״̬
typedef enum
{
	member_s_offline,
	member_s_online,
	member_s_idle,
	member_s_away,
	member_s_busy,
} MemberStatus;

LIB_MSN_API const char* StatusToString(MemberStatus status);

//class Member;
//class MemberCmp
//{
//public:
//	MemberCmp();
//	~MemberCmp();
//
//	bool operator()(const Member* a, const Member* b) const;
//protected:
//private:
//};

class CMsnContact;

// ��ϵ�˳�Ա������Ͷ���
class LIB_MSN_API Member
{
public:
	Member();
	~Member();

	// from xml data

	// for PassportName
	char* DisplayName;   // ��ʾ��
	char* PassportName;  // �ʺ�
	bool  IsPassportNameHidden;
	int   PassportId;
	char* CID;
	bool  LookedupByCID;

	// for Email
	char* Email;  // �����ַ

	// for Phone
	char* Phone;

	// ͨ����
	int   MembershipId;
	char* Type;
	char* State;
	bool  Deleted;
	char* LastChanged;
	char* JoinedDate;
	char* ExpirationDate;

	// added
	MsnListOp  op_;
	bool authorized_;       // �����ַ�Ƿ�������֤
	MemberStatus online_;   // ����״̬

	CMsnContact* contact_;  // ��������ϵ�˶���
	void SetContact(CMsnContact* contact);

	// ����ó�Ա����Ϣ
	void Out(void) const;
};

struct ACL_XML;
class acl::charset_conv;

class LIB_MSN_API CMembership
{
public:
	/**
	* ���캯�������ݴ���ĺ��� MemberShip ���ݵ� xml ��㣬������
	* �� MemberShip �еĳ�Ա����
	* @param xmlMemberShip {ACL_XML&} ĳ�� MemberShip ����Լ���
	*  �ӽ����ɵ� xml ������
	* @param conv {acl::charset_conv*} ����ǿգ��ɶԽ�������ø�
	*  ת���������ַ�����ת��
	*/
	CMembership(ACL_XML& xmlMembership, acl::charset_conv* conv);
	~CMembership(void);

	// ����ֵ��Ҫ�� STRING_IS_EMPTY �ж��Ƿ��ǿ�ֵ
	const char* GetMemberRole() const
	{
		return MemberRole_;
	}

	const std::list<Member*> GetMembers(void) const
	{
		return members_;
	}

	MsnListId GetListId() const
	{
		return listId_;
	}

	MsnListOp GetListOp() const
	{
		return listOp_;
	}

	void Out(void) const;
private:
	char* MemberRole_;
	MsnListId listId_;
	MsnListOp listOp_;
	std::list<Member*> members_;

	/**
	* ���� XML ���ݽ�㣬���� Member ����
	* @param xmlMember {ACL_XML*} �� Member ���Ϊ���� XML ��
	* @param conv {acl::charset_conv*} �ַ���ת�������ǿ�ʱ���ø�
	*  ת���������ַ���ת��
	* @param listOp {MsnListOp}
	*/
	void AddMember(ACL_XML* xmlMember, acl::charset_conv* conv, MsnListOp listOp);

	/**
	* ���ó�Ա��ͨ������
	* @param xmlMember {ACL_XML*} �� Member ���Ϊ���� XML ��
	* @param member {Member*} ��ϵ�˳�Ա
	*/
	void SetMemberCommon(ACL_XML* xmlMember, Member* member);

	/**
	* ��� Passport ���͵���ϵ�˳�Ա����
	* @param xmlMember {ACL_XML*} �� Member ���Ϊ���� XML ��
	* @param conv {acl::charset_conv*} �ַ���ת����
	* @return {Member*} Passport ���͵���ϵ�˶���
	*/
	Member* GetPassportMember(ACL_XML* xmlMember, acl::charset_conv* conv);

	/**
	* ��� Email ���͵ĳ�Ա����
	* @param xmlMember {ACL_XML*} �� Member ���Ϊ���� XML ��
	* @return {Member*} Email ���͵���ϵ�˶���
	*/
	Member* GetEmailMember(ACL_XML* xmlMember);

	/**
	* ��� Phone ���͵ĳ�Ա����
	* @param xmlMember {ACL_XML*} �� Member ���Ϊ���� XML ��
	* @return {Member*} Phone ���͵���ϵ�˶���
	*/
	Member* GetPhoneMember(ACL_XML* xmlMember);
};

struct ACL_ARRAY;

class LIB_MSN_API CMsnMemberShips
{
public:
	CMsnMemberShips(void);
	~CMsnMemberShips(void);

	/**
	* ���ݴ���� MemberShip xml ��㼯�ϴ��� MemberShip ���϶���,
	* �����Ķ�����Ҫ delete ��
	* @param memberShipArray {ACL_ARRAY*} �� ACL_XML_NODE ��ɵ����飬ÿ��XML
	*  �������ṩ��һ�� Membership ��ԭʼ������Ϣ
	* @return {CMsnMemberShipContacts*} �������Ҫ delete �ͷ�
	*/
	static CMsnMemberShips* Create(ACL_ARRAY* memberShipArray);

	// ������е� Membership �����б�
	std::list<CMembership*>& GetMemberShips(void);

	// �����Ҫ��ʾ���û��б�
	//std::set<Member*, MemberCmp>& GetUsers(void);
	//std::list<Member*>& GetUsers(void);

	// ���Խӿڣ����������ϵ���б�
	void Out(void) const;
private:
	std::list<CMembership*> memberships_;
	//std::set<Member*, MemberCmp> users_;
	//std::list<Member*> users_;
	//bool user_builded_;
};
