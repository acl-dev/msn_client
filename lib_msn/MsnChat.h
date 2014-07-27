#pragma once
#include "string.hpp"
#include "aio_socket_stream.hpp"
#include "ofstream.hpp"
#include "lib_msn.h"

class LIB_MSN_API CMsnChat : public acl::aio_open_callback
{
public:
	CMsnChat(const char* class_name = "unknown");
	virtual ~CMsnChat(void);

	/**
	* �첽����Զ�̷�������ͬʱ�첽��������������
	* �ù�����Ϊ�������������첽�������̣��������ӵĽ����ͨ���ص�
	* ��֪ͨ��
	* @param domain {const char*} ����������(Ҳ������IP��ַ)
	* @param port {unsigned short} �������˿�
	* @param timeout {int} ���ӳ�ʱʱ��
	*/
	void Open(const char* domain, unsigned short port, int timeout);

	/**
	* �첽����Զ�̷���������ַ��ʽΪ��ip:port����ȻҲ������: domain:port
	* ����������µ�����������ͬ���ģ��ᵼ��������������
	* @param addr {const char*} ��������ַ����ʽ��ip:port
	* @param timeout {int} ���ӳ�ʱʱ��
	* @return {bool} ������� false ��˵��һ���ͳ����������Ӵ�������
	*  ִ�й����У����ӵĳɹ�������ڻص���֪ͨ��
	*/
	bool Open(const char* addr, int timeout);

	/**
	* �ر�����
	*/
	void Close();

	/**
	* ��������
	* @param data {const char*} ����ָ���ַ
	* @param len {size_t} ���ݳ���
	* @return {bool} �������δ�򿪵��ô˺����򷵻� false
	*/
	bool Send(const char* data, size_t len);

	/**
	* ��һ������
	* @param nonl {bool} �Ƿ��Զ�ȥ��\r\n
	* @return {bool} �������δ�򿪵��ô˺����򷵻� false
	*/
	bool Gets(bool nonl = true);

	/**
	* ���涨���ȵ����ݻ������ݿɶ��Ͷ�
	* @param len {int} ��ȡ���ݵĳ��ȣ������ֵΪ0���ʾ�����ݿɶ���
	*  �����ݲ����ض��������ݼ�����
	* @return {bool} �������δ�򿪵��ô˺����򷵻� false
	*/
	bool Read(int len = 0);

	/**
	* ��DNS��ѯ���غ�Ļص�����
	* @param ip {const char*} ��ѯ����IP��ַ
	*/
	void OnDnsResult(const char* ip);

	/**
	* �����������ĵ���״̬
	* @param onoff {bool}
	*/
	void Debug(bool onoff);
protected:
	//////////////////////////////////////////////////////////////////////////
	// �麯��

	/**
	* ��Զ�����ӷ������ɹ�����麯��
	*/
	virtual void OnOpen() {}

	/**
	* ������Ҫ������ݺ�Ļص�����
	* @param data {char*} ���ݵ�ַָ��
	* @param len {int} ���ݳ���
	*/
	virtual void OnRead(char* data, int len) {}

	/**
	* �����ͳɹ���Ļص�����
	*/
	virtual void OnSend() {}

	/**
	* �����ӳ�ʱ�����ʱ�Ļص�����
	*/
	virtual void OnTimeout() {}

	/**
	* �����ӹر�ʱ�Ļص�����
	*/
	virtual void OnClose() {}
private:
	// ���� aio_open_callback ���麯��
	virtual bool read_callback(char* data, int len);
	virtual bool write_callback();
	virtual void close_callback();
	virtual bool timeout_callback();
	virtual bool open_callback();

	acl::string class_name_;
	acl::string addr_;
	acl::aio_socket_stream* stream_;
	unsigned short port_;
	int conn_timeout_;
	bool debug_;
	acl::ofstream* debug_fpout_;
	bool opened_;

	void logger_format(const char* fmt, ...);
};
