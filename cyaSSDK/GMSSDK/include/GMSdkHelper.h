#ifndef __GM_SDK_HELPER_H__
#define __GM_SDK_HELPER_H__

#include "cyaBase.h"	//文件基础;
#include "SemaDataMan.h"
#include "cyaLocker.h"	//同步对象_线程锁;
#include "cyaLog.h"
#include "cyaCoreTimer.h"	//Timer
#include "GMProtocal.h"	//查询管理服务器信息结构;
#include <map>

class GMSdkSesssion;
class CGMSdkHelper
{
public:
	CGMSdkHelper(const char* gmsIp,
		UINT16 gmsPort,
		UINT16 connTimeout = 5000,
		UINT16 readTimeout = 0,
		UINT16 writeTimeout = 0
		);
	~CGMSdkHelper();
	///查统计实时新增数据;
	TimeAddedinfo* QueryRealTimeAddStatistics(UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);

	//查询充值订单详情;
	RecharegeStatisticeIofo* QueryOrderTracking(UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);

	//查询充值数据统计;
	StartisticsInfo* QueryPecharGeStatistcs(UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);
	//查询留存统计(次日、三日、七日、第三十日、月)
	GMQLeavekeepDateInfo* QueryLeaveKeepStatistics(UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endTime, BYTE lktype, BYTE lkTimeType, BYTE& retNum, UINT16* retCode, UINT32* total);
	//查询实时在线;
	OnlineInfo* QueryOnlineCurrent(UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* startTime, BYTE& retNum, UINT16* retCode, UINT32* total);
	//等级分布统计;
	GM_Grade_RatingStatisticsInfo* QueryGrade_RatingStatisticsInfo(UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);

public:
	void InitFunc();
	///登录
	int Login();
	///退出
	void Loginout();
	///设置本地会话
	void SetLocalSession(GMSdkSesssion* session);
	///写入进程数据
	void InputProcessData(UINT16 packType, void* data, int dataLen, DWORD req, BOOL isOver);
	///释放handle
	void  ReleaseHandle();
	///启动session重连
	void StartReconnect();

	///更新心跳时间
	void UpdateHeatbeatTime()
	{
		m_heartbeatTime = GetTickCount();
		LogInfo(("Client心跳检测\n"));
	}

	///重连Timer触发函数
	static void ReconnectTimer(void* param, TTimerID id);
	///心跳Timer触发函数
	static void HeartbeatTimer(void* param, TTimerID id);

private:

	long AddOneForRequence();
	BOOL IsNull(void* p)
	{
		if (p == NULL)
			return true;
		return false;
	}


	///发送数据
	///@cmdCode[IN]: 命令码
	///@req[IN]: 包序列号
	///@buf[IN: 发送数据
	///@bufLen[IN]: 发送数据长度
	///isWait[IN]:是否等待
	///outData[OUT]: 返回数据
	///@outDataLen[OUT]: 返回数据长度
	int SendDataMan(UINT16 cmdCode, DWORD req, void* buf, int bufLen, BYTE isWait, void*& outData, UINT32* outDataLen);

	///数据包加密及发送长度处理
	///@isCrypt[IN]: 是否加密
	///@buf[OUT]:加密数据
	///@unCrptfLen[IN]:原始数据长度
	///@sourceHead[IN]:数据协议头长度
	///@key[IN]: 加密key
	///@crptLen[OUT]:发送数据长度
	void EncryptPakcge(BYTE isCrypt, BYTE*& buf, UINT16 unCrptfLen, UINT16 sourceHead, char* key, UINT16* crptLen);

	///发送心跳包处理
	void SendHeartPackge();
	///正在重连
	void OnReconnect();
	///心跳检测
	void OnHeartbeat();

private:
	typedef std::map<UINT32, CSemaDataMan*> SemaData;
	typedef int (CGMSdkHelper::*FuncProcessManHandle)(UINT16 plat, UINT16 partId, const char* startTime, const char* endTime, int* getCount);
	std::map<BYTE, FuncProcessManHandle> m_funcHanlde;


	char m_gmsIp[17];			//管理服务器IP
	UINT16 m_gmsPort;			//管理服务器端口
	UINT16 m_connTimeout;		//连接超时

	SemaData m_semaData;		//包序列号集
	CXTLocker m_locker;
	long m_requence;			//初始包序列号
	char m_key[33];			//密锁
	BOOL m_isLogin;			//是否登录

	GMSdkSesssion* m_pLocalSession; //本地会话
	CyaCoreTimer m_reconnectTimer; //重连Timer
	CyaCoreTimer m_heartbeatTimer; //心跳Timer
	DWORD m_heartbeatTime; //心跳时间
	DWORD m_heartbeatInterval; //心跳间隔时间

	GMProtocalHead m_hearbeatHead;



};


#endif