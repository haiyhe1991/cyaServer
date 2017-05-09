#ifndef __LINK_CONFIG_H__
#define __LINK_CONFIG_H__

#include <string>
#include "cyaTypes.h"

#define SEPARATOR(x) 1

class LinkConfig
{
public:
	LinkConfig();
	~LinkConfig();

	/// ��ʼ��������Ϣ
	int InitConfig(const PathName cfgPath);

	///*********** ��ȡLinkServer��Ϣ **************/
	BOOL GetLinkInvalidIP() const;
	/// LinkServer��ַ
	const char* GetLinkAddr() const;
	/// LinkServer�˿�
	int GetLinkListenPort() const;
	/// LinkServer�ı��
	int GetLinkNumber() const;
	/// �ͻ���������ʱʱ�� ��λ(����)
	int GetClientHeartBeatTimeout() const;

	///*********** ��ȡ���ط�������Ϣ **************/
	const char* GetGWSAddr() const;
	int GetGWSPort() const;

	/// ��ȡ��֤��������Ϣ
	const char* GetLSAddr() const;
	int GetLSPort() const;
	/// ��ȡ��֤������ͨ�ŵ�Token
	const char* GetLSToken() const;
	/// �ϱ�������ʱ���� ��λ(����)
	int GetLSReportInvterval() const;
	/// ����ʱToken��ʱ���� ��λ(����)
	int GetLinkCleanInvterval() const;

	/// ��ȡ���������������Ϣ
	const char* GetManAddr() const;
	int GetManPort() const;

	/// ��ȡ����ID
	int GetLinkerPartitionID() const;
private:
	bool ReadConfigFile(const PathName cfgPath);

private:
	int m_LinkPort; /// LinkServer�˿�
	int m_LinkNum;  /// LinkServer�ķ��������
	int m_GWSPort;  /// ���ط������˿�
	int m_LSPort;   /// ��֤�������˿�
	int m_ManPort;  /// �����������˿�
	int m_LSReport; /// �ϱ�LinkServer�ͻ���������deʱ����(��λ:����)
	int m_PartitionId;  /// ��������ID
	int m_LinkHeartSec; /// �������
	int m_LinkClean;    /// �ͻ��˳�ʱʱ�� ����
	BOOL m_LinkIpInvalid;   /// LinkServer��ַ�������ϵ�IPʱΪfalse(default), ���������ϵ�IPʱΪtrue

	std::string m_LinkAddr; /// LinkServer��ַ
	std::string m_GWSAddr;  /// ���ط�������ַ
	std::string m_LSAddr;   /// ��֤��������ַ
	std::string m_ManAddr;  /// ���������ַ
	std::string m_LSToken;  /// ��֤��������֤��Token
};

#endif