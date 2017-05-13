#ifndef __ROOM_CONFIG_H__
#define __ROOM_CONFIG_H__

#include "cyaTypes.h"

class CRoomConfig
{
public:
	static CRoomConfig* FetchConfigInstance();
	static void ReleaseConfigInstance();

	/// ���ر��ؼ����˿�
	INT GetLocalListenPort() const;
	/// ���ط�������������˿�
	INT GetRoomServerPort() const;
	/// ���ط����������ַ
	DWORD GetRoomServerAddr() const;
	const char* GetRoomServerStrAddr() const;

	/// ���ط�������ַ�Ͷ˿�
	INT GetGWServerPort() const;
	const char* GetGWServerAddr() const;
private:
	CRoomConfig();
	~CRoomConfig();

	CRoomConfig(const CRoomConfig&);
	CRoomConfig& operator=(const CRoomConfig&);
private:
	void InitializeConfig();
private:
	static CRoomConfig* m_this;
	INT m_nLocalListenPort;				/// ���ؼ����˿�
	INT m_nRoomServerPort;				/// ����������˿�
	INT m_nGWServerPort;				/// ���ط������˿�

	std::string m_strRoomServerAddr;	/// �����������ַ
	std::string m_strGWServerAddr;		/// ���ط�������ַ
};

#endif