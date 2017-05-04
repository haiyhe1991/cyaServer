#ifndef __CSERVE_CONN_MAN_H__
#define __CSERVE_CONN_MAN_H__

#include "DBSSdk.h"

#include <vector>
#include <map>
#include "cyaLocker.h"

///Linker连接后的socket 和key
struct LinkSocketKey
{
	SOCKET sock; //连上linker的socket
	char key[33]; //密钥(收发数据加解密)
};

typedef std::vector<LinkSocketKey/*socket和key信息*/> LKSocketKey; //Linker 连接信息(保存连接的socket和加密key)
typedef std::map<UINT16/*所属分区ID*/, LKSocketKey/*socket和key容器*/> PartLkSockKey; //分区连接信息

class CServeConnMan
{
public:
	CServeConnMan();
	~CServeConnMan();

	int InitConn();
	void DestroyConn();

	///获取Link服务器连接
	void GetLinkSocketKey(UINT16 platId/*平台ID*/, UINT16 partId/*分区ID*/, PartLkSockKey& sockKey);
	///获取平台分区数据库服务器连接
	void GetDBShandle(UINT16 platId/*平台ID*/, UINT16 partId/*分区ID*/, std::map<UINT16/*分区ID*/, DBSHandle/*分区数据库handle*/>& dbsHandle);

	///获取充值数据库服务器连接
	void GetRechargeDBShandle(DBSHandle& handle)
	{
		handle = m_rechargeDBSHanle;
	}
	///获取帐号数据库服务器连接
	void GetAccountDBShandle(DBSHandle& handle)
	{
		handle = m_accountDBSHanle;
	}



private:
	///初始化平台DBServer 连接
	int InitPlatformDBServeHandle();
	///初始化平台Linker 连接
	int InitPlatformLinkerServeConn();
	///初始化充值DBServer 连接
	int InitRechargeDBServeHandle();
	///初始化帐号DBServer 连接
	int InitAccountDBServeHandle();

	///销毁平台DBServer 连接
	void DestroyPlatformDBServeHandle();
	///销毁平台Linker 连接
	void DestroyPlatformLinkerServeConn();
	///销毁充值DBServer 连接
	void DestroyRechargeDBServeHandle();
	///销毁帐号DBServer 连接
	void DestroyAccountDBServeHandle();


private:
	int  LoginLinker(const char* ip, int port, SOCKET* sockt, char key[33]);

private:
	typedef std::map<UINT16/*平台ID*/, PartLkSockKey/*平台linker socket和key*/> PlatLkSockKey; //平台分区连接信息
	typedef std::map<UINT16/*所属分区ID*/, DBSHandle/*分区数据库Handle*/> PartDBSHandle;	//分区数据库服务器连接信息
	typedef std::map<UINT16/*平台ID*/, PartDBSHandle/*平台数据库Handle*/> PlatDBSHandle;	//平台分区数据库服务器连接信息

	PlatLkSockKey m_platLkSocketKey; //平台分区连接Sockt和key
	PlatDBSHandle m_platDBShandle;   //平台分区数据库服务器handle
	DBSHandle m_rechargeDBSHanle;    //充值数据库服务器handle
	DBSHandle m_accountDBSHanle;    //帐号数据库服务器handle

	CXTLocker m_connLocker;
	CXTLocker m_dbconnLocker;


};

int InitServeConnObj();

CServeConnMan* GetServeConnObj();

void DestroyServeConnObj();

#endif