// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
//��ı�׼�������� DLL �е������ļ��������������϶���� LIB_MSN_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
//�κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ 
// LIB_MSN_API ������Ϊ�ǴӴ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

#pragma once

#ifdef LIB_MSN_EXPORTS
#define LIB_MSN_API __declspec(dllexport)
#else
#define LIB_MSN_API __declspec(dllimport)
#endif

// �����Ǵ� lib_msn.dll ������
class LIB_MSN_API Clib_msn {
public:
	Clib_msn(void);
	// TODO: �ڴ�������ķ�����
};

extern LIB_MSN_API int nlib_msn;

LIB_MSN_API int fnlib_msn(void);
