#ifndef __ROOM_CONFIG_H__
#define __ROOM_CONFIG_H__

#include "cyaTypes.h"

class CRoomConfig
{
public:
	static CRoomConfig* FetchConfigInstance();
	static void ReleaseConfigInstance();

	/// 返回本地监听端口
	INT GetLocalListenPort() const;
	/// 返回房间服务器监听端口
	INT GetRoomServerPort() const;
	/// 返回房间服务器地址
	DWORD GetRoomServerAddr() const;
	const char* GetRoomServerStrAddr() const;

	/// 网关服务器地址和端口
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
	INT m_nLocalListenPort;				/// 本地监听端口
	INT m_nRoomServerPort;				/// 房间服务器端口
	INT m_nGWServerPort;				/// 网关服务器端口

	std::string m_strRoomServerAddr;	/// 房间服务器地址
	std::string m_strGWServerAddr;		/// 网关服务器地址
};

#endif