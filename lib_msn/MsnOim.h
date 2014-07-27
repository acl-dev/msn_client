#pragma once
#include "lib_msn.h"

class acl::mime;

class LIB_MSN_API CMsnOim
{
public:
	CMsnOim(const acl::mime& mime);
	~CMsnOim();

	int inbox_total_;
	int inbox_unread_;
	int other_total_;   // Sent + Junk + Drafts_;
	int other_unread_;
	int QTM;
	int QNM;

	char* inbox_url_;
	char* folders_url_;
	char* post_url_;

	/**
	* ����������Ϣ
	* @param mime {const acl::mime&} ������
	* @return {CMsnOim*} �������ֵΪ�գ���˵��
	*/
	static CMsnOim* Create(const acl::mime& mime);
protected:
private:

};