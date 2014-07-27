#pragma once
#include <list>
#include <set>
#include "lib_msn.h"

class acl::charset_conv;
class CMsnContact;

class LIB_MSN_API CMsnGroup
{
public:
	CMsnGroup(ACL_XML_NODE* group, acl::charset_conv& conv);
	~CMsnGroup(void);

	char* groupId;
	bool propertiesChanged;
	bool fDeleted;
	char* lastChange;

	// of groupInfo
	char* groupType;
	char* name;
	bool IsNotMobileVisible;
	bool IsPrivate;
	bool IsFavorite;

	// ��Ӵ����ڸ���ĳ�Ա
	bool AddContact(CMsnContact* contact);

	// ����������Ϣ
	void Out(void) const;
private:
	// �����ڸ�����û���Ա�б�
	std::set<CMsnContact*> contacts_;
};

struct ACL_ARRAY;

class LIB_MSN_API CMsnGroups
{
public:
	CMsnGroups(void);
	~CMsnGroups(void);

	static CMsnGroups* Create(ACL_ARRAY* groupsArray);

	std::list<CMsnGroup*>& GetGroups(void)
	{
		return (groups_);
	}

	// ������ID���������
	CMsnGroup* GetGroup(const char* guid) const;

	void Out(void) const;
protected:
private:
	std::list<CMsnGroup*> groups_;
};
