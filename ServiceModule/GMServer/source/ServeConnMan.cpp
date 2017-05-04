#include "ServeConnMan.h"
#include "GMCfgMan.h"
#include "cyaSock.h"
#include "cyaIpcvt.h"
#include "cyaLog.h"
#include "GMProtocal.h"
#include "ServiceProtocol.h"
#include "ServiceCommon.h"
#include "ServiceErrorCode.h"
#include "GMComm.h"
#include "AES.h"

CServeConnMan* g_pserveconnObj = NULL;


CServeConnMan::CServeConnMan()
{
	SockInit();
	DBS_Init();
}


CServeConnMan::~CServeConnMan()
{
	DestroyConn();
}

int CServeConnMan::InitConn()
{
	if (InitPlatformDBServeHandle())
		return 1;

	if (InitPlatformLinkerServeConn())
		return 1;

	if (InitRechargeDBServeHandle())
		return 1;

	if (InitAccountDBServeHandle())
		return 1;


	return 0;
}

void CServeConnMan::DestroyConn()
{
	DestroyPlatformDBServeHandle();
	DestroyPlatformLinkerServeConn();
	DestroyRechargeDBServeHandle();
	DestroyAccountDBServeHandle();
}

void CServeConnMan::GetLinkSocketKey(UINT16 platId/*平台ID*/, UINT16 partId/*分区ID*/, PartLkSockKey& sockKey)
{
	CXTAutoLock locked(m_connLocker);
	PlatLkSockKey::iterator it = m_platLkSocketKey.find(platId);
	if (it != m_platLkSocketKey.end())
	{
		PartLkSockKey::iterator its;
		if (partId != 0) //获取指定平台的分区
		{
			its = it->second.find(partId);
			if (its != it->second.end())
				sockKey.insert(std::make_pair(its->first, its->second));
		}
		else //获取平台所有分区
		{
			its = it->second.begin();
			for (its; its != it->second.end(); its++)
				sockKey.insert(std::make_pair(its->first, its->second));
		}
	}
	else
		return;

}

void CServeConnMan::GetDBShandle(UINT16 platId/*平台ID*/, UINT16 partId/*分区ID*/, std::map<UINT16/*分区ID*/, DBSHandle/*分区数据库handle*/>& dbsHandle)
{
	CXTAutoLock locked(m_dbconnLocker);
	PlatDBSHandle::iterator it = m_platDBShandle.find(platId);
	if (it != m_platDBShandle.end())
	{
		PartDBSHandle::iterator its;
		if (partId != 0) //获取指定平台指定分区数据库服务器handle
		{
			its = it->second.find(partId);
			if (its != it->second.end())
			{
				if (its->second != NULL)
					dbsHandle.insert(std::make_pair(partId, its->second));
			}
		}
		else //获取指定平台的所有分区数据库服务器handle
		{
			its = it->second.begin();
			for (its; its != it->second.end(); its++)
				dbsHandle.insert(std::make_pair(its->first, its->second));
		}
	}
}

int CServeConnMan::InitPlatformDBServeHandle()
{
	CXTAutoLock locked(m_dbconnLocker);
	ChannelDBServeInfo platdbServeInfo = GetCfgManObj()->GetPlatDBServeInfo();
	ChannelDBServeInfo::iterator it = platdbServeInfo.begin();
	for (it; it != platdbServeInfo.end(); it++)
	{
		PartDBSHandle partDbsHanle;
		DBSServeInfo dbsInfo = it->second;
		DBSServeInfo::iterator its = dbsInfo.begin();

		for (its; its != dbsInfo.end(); its++)
		{
			DBSHandle handle = DBS_Connect(its->second.sIp, its->second.sPort);
			int num = 0;
			while (handle == NULL)
			{
				handle = DBS_Connect(its->second.sIp, its->second.sPort);
				LogInfo(("无法连接DBServer[ip: %s, port: %d]", its->second.sIp, its->second.sPort));
				num++;
				if (num == 3)
					break;
				Sleep(1000);
			}
			PartDBSHandle::iterator it1 = partDbsHanle.find(its->first);
			if (it1 != partDbsHanle.end())
				ASSERT(false); //当前平台的分区ID重复
			partDbsHanle.insert(std::make_pair(its->first, handle));
		}
		PlatDBSHandle::iterator it2 = m_platDBShandle.find(it->first);
		if (it2 != m_platDBShandle.end())
			ASSERT(false); //平台ID重复
		m_platDBShandle.insert(std::make_pair(it->first, partDbsHanle));
	}
	return 0;
}


int CServeConnMan::InitPlatformLinkerServeConn()
{
	CXTAutoLock locked(m_connLocker);
	ChannelLinkerServeInfo platLkServeInfo = GetCfgManObj()->GetPlatLinkerServeInfo();
	ChannelLinkerServeInfo::iterator it = platLkServeInfo.begin();
	for (it; it != platLkServeInfo.end(); it++)
	{
		//平台分区处理
		PartLkSockKey partLkSockKey;
		LKServeInfo lkServeInfo = it->second;
		LKServeInfo::iterator its = lkServeInfo.begin();
		for (its; its != lkServeInfo.end(); its++)
		{
			//分区处理
			LKSocketKey plkSockKey;
			ServeInfoVec serveInfo = its->second;
			for (int i = 0; i < (int)serveInfo.size(); i++)
			{
				LinkSocketKey socktKey;

				SOCKET sock = 0;
				char key[33] = { 0 };

				int ret = LoginLinker(serveInfo[i].sIp, serveInfo[i].sPort, &sock, key);
				if (ret == GM_OK)
				{
					plkSockKey.push_back(socktKey);
					plkSockKey[i].sock = sock;
					strcpy(plkSockKey[i].key, key);
				}
				else
					continue;
			}
			PartLkSockKey::iterator it2 = partLkSockKey.find(its->first);
			if (it2 != partLkSockKey.end())
				ASSERT(false); //当前分区重复
			partLkSockKey.insert(std::make_pair(its->first, plkSockKey));
		}
		PlatLkSockKey::iterator it3 = m_platLkSocketKey.find(it->first);
		if (it3 != m_platLkSocketKey.end())
			ASSERT(false); //当前平台重复
		m_platLkSocketKey.insert(std::make_pair(it->first, partLkSockKey));
	}
	return 0;
}


int CServeConnMan::InitRechargeDBServeHandle()
{
	ServeInfo dbsInfo = GetCfgManObj()->GetRechargeDBServeInfo();
	DBSHandle handle = DBS_Connect(dbsInfo.sIp, dbsInfo.sPort);
	while (handle == NULL)
	{
		handle = DBS_Connect(dbsInfo.sIp, dbsInfo.sPort);
		Sleep(5);
	}
	m_rechargeDBSHanle = handle;
	return 0;
}

int CServeConnMan::InitAccountDBServeHandle()
{
	ServeInfo dbsInfo = GetCfgManObj()->GetAccountDBServeInfo();
	DBSHandle handle = DBS_Connect(dbsInfo.sIp, dbsInfo.sPort);
	while (handle == NULL)
	{
		handle = DBS_Connect(dbsInfo.sIp, dbsInfo.sPort);
		Sleep(5);
	}
	m_accountDBSHanle = handle;
	return 0;
}

///销毁平台DBServer 连接
void CServeConnMan::DestroyPlatformDBServeHandle()
{
	CXTAutoLock locked(m_connLocker);
	PlatDBSHandle::iterator it = m_platDBShandle.begin();
	for (it; it != m_platDBShandle.end();)
	{
		PartDBSHandle dbHandle = it->second;
		PartDBSHandle::iterator it1 = dbHandle.begin();
		for (it1; it1 != dbHandle.end();)
		{
			DBSHandle handle = it1->second;
			DBS_DisConnect(handle);

			if (dbHandle.size() <= 0)
				break;
			else
				dbHandle.erase(it1++);
		}
		if (m_platDBShandle.size() <= 0)
			break;
		else
			m_platDBShandle.erase(it++);
	}
}

///销毁平台Linker 连接
void CServeConnMan::DestroyPlatformLinkerServeConn()
{
	CXTAutoLock locked(m_connLocker);
	PlatLkSockKey::iterator it = m_platLkSocketKey.begin();
	for (it; it != m_platLkSocketKey.end();)
	{
		PartLkSockKey partlkSockkey = it->second;
		PartLkSockKey::iterator it1 = partlkSockkey.begin();
		for (it1; it1 != partlkSockkey.end();)
		{
			LKSocketKey::iterator it2 = it1->second.begin();
			for (it2; it2 != it1->second.end();)
			{
				SockClose(it2->sock);
				if (it1->second.size() <= 0)
					break;
				else
					it1->second.erase(it2++);
			}
			if (partlkSockkey.size() <= 0)
				break;
			else
				partlkSockkey.erase(it1++);
		}
		if (m_platLkSocketKey.size() <= 0)
			return;
		else
			m_platLkSocketKey.erase(it++);
	}
}

///销毁充值DBServer 连接
void CServeConnMan::DestroyRechargeDBServeHandle()
{
	DBS_DisConnect(m_rechargeDBSHanle);
}


///销毁充值DBServer 连接
void CServeConnMan::DestroyAccountDBServeHandle()
{
	DBS_DisConnect(m_accountDBSHanle);
}




int  CServeConnMan::LoginLinker(const char* ip, int port, SOCKET* sockt, char key[33])
{
	SGSProtocolHead* sgsProHead = NULL;
	char head[65] = { 0 };
	char buf[1024] = { 0 };
	char skey[33] = { 0 };

	memset(key, 0, 33);
	memset(skey, 0, 33);

	///设置默认加密key
	strcpy(skey, "CYA_DEFAULT_TOKEN_KEY_1234567890");
	SOCKET sock = TCPNewClientByTime(GetDWordIP(ip), port, 3000);
	if (!SockValid(sock))
	{
		LogInfo(("无法连接Linker服务器[ip: %s, port: %d]!\n", ip, port));
		return GM_INVALID_SOCKET_CONN_LINKER;
	}

	int res = SockReadAllByTime(sock, head, GET_SGS_PROTOHEAD_SIZE, 3000);
	if (res <= 0)
		return GM_READ_LINKER_DATA_TIMEOUT;

	sgsProHead = (SGSProtocolHead*)head;
	ASSERT(sgsProHead != NULL);
	sgsProHead->ntoh();
	res = SockReadAllByTime(sock, buf, sgsProHead->pduLen, 2000);
	if (res <= 0)
		return GM_READ_LINKER_DATA_TIMEOUT;
	if (sgsProHead->isCrypt)
		appDecryptDataWithKey((AES_BYTE*)buf, sgsProHead->pduLen, (ANSICHAR*)skey);

	///获取加密key
	SGSResPayload *presPayload = (SGSResPayload*)buf;
	presPayload->ntoh();
	if (presPayload->cmdCode == DATA_TOKEN_KEY_PACKET && presPayload->retCode == 0)
	{
		SConnectLinkerRes* p = (SConnectLinkerRes*)presPayload->data;
		p->ntoh();
		memcpy(skey, p->key, 32);
		skey[32] = '\0';
	}
	else
		return GM_PARSE_KEY_FAILED;
	memcpy(key, skey, 32);

	*sockt = sock;
	return GM_OK;
}

int InitServeConnObj()
{
	if (g_pserveconnObj != NULL)
		return 1;

	g_pserveconnObj = new CServeConnMan();
	ASSERT(g_pserveconnObj != NULL);
	return g_pserveconnObj->InitConn();
}

CServeConnMan* GetServeConnObj()
{
	return g_pserveconnObj;
}

void DestroyServeConnObj()
{
	if (g_pserveconnObj == NULL)
		return;

	g_pserveconnObj->DestroyConn();
	delete g_pserveconnObj;
	g_pserveconnObj = NULL;
}