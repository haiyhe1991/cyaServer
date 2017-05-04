#ifndef __EXT_SERVE_LOGIC_MAN_H__
#define __EXT_SERVE_LOGIC_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
管理服务器逻辑处理
*/

#include <map>
#include "cyaTypes.h"

class LicenceSession;
class ExtServeLogicMan
{
public:
	ExtServeLogicMan();
	~ExtServeLogicMan();

public:
	int ProcessLogic(LicenceSession* licSession, UINT16 cmdCode, const void* payload, int nPayloadLen, BYTE pktType);

private:
	//查询用户区
	int OnQueryUserPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//查询玩家账户
	int OnQueryPlayerAccount(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//查询游戏分区
	int OnQueryGamePartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//增加分区
	int OnAddPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//删除分区
	int OnDelPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//修改分区名字
	int OnModPartitionName(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//设置推荐分区
	int OnSetRecommandPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//开启分区服务请求(DATA_PACKET)
	int OnStartPartitionServe(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//停止分区服务请求(DATA_PACKET)
	int OnStopPartitionServe(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//开启link服务响应包(DATA_CTRL_PACKET)
	int OnStartLinkServeResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//停止link服务响应包(DATA_CTRL_PACKET)
	int OnStopLinkServeResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen);

private:
	typedef int (ExtServeLogicMan::*fnExtManProcessHandler)(LicenceSession* licSession, const void* payload, int nPayloadLen);
	std::map<UINT16, fnExtManProcessHandler> m_handlerMap;
};

void InitExtServeLogicMan();
ExtServeLogicMan* GetExtServeLogicMan();
void DestroyExtServeLogicMan();

#endif