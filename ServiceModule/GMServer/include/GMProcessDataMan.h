#ifndef __GM_PROCESS_DATA_MAN_H__
#define __GM_PROCESS_DATA_MAN_H__

#include <map>

#include "cyaBase.h"
#include "cyaTcp.h"
#include "GMprotocal.h"
#include "GMCfgMan.h"
#include <vector>

class CCMSession;

class CGMProcessDataMan
{
public:
	CGMProcessDataMan();
	~CGMProcessDataMan();

	/*进程消息数据处理*/
	int OnMsgProcessDataMan(UINT16 cmdCode, void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---实时新增统计---*/
	int	QueryNewStatistics(void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---实时充值统计---*/
	///订单查询;			
	int QueryRechargeOrder(void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---充值数据统计---*/
	int QueryPecharGeStatistcs(void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---留存数据统计---*/
	int QueryLeaveKeepStatistics(void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---在线人数统计---*/
	int QueryRealtimeoneline(void* data, int datalen, CCMSession* session, DWORD sreq);

	/*---在线数据统计---*/
	/*int QueryOnlineNumStatistics(void* data,int dataLen,CCMSession* session, DWORD sreq);*/

	/*---等级分布统计---*/
	int QueryLevelDistribution(void* data, int dataLen, CCMSession* session, DWORD sreq);

	//======================================================================================
private:

	int ProcessDataSend(CCMSession* session, void* data, int dataLen);										///		返回数据发送;	

	void RechargeOrder(std::vector<RecharegeStatisticeIofo>& OrderInfo, RecharegeStatisticeIofo data[1]);	///		充值订单详细信息处理;

	void RecharegeStat(std::vector<StartisticsInfo>& OrderInfo, StartisticsInfo data[1]);					///		充值统计最后返回取值处理;

private:
	/*内存分配处理*/
	void* AllocMan(int nSize)
	{
		return CyaTcp_Alloc(nSize + 1);
	}
	/*内存释放处理*/
	void FreeMan(void* p)
	{
		if (p != NULL)
		{
			CyaTcp_Free(p);
			p = NULL;
		}
	}

	BOOL IfDayDateTime(const char* startTime);											///	是否是当前日期，如果是就返回	return TRUE;return FALSE;

	int IfMinuteTime(char* startTime);													///	是否是当前时间，时针，分钟10分钟一类则为真;

	void Timetodefine(char* startTIme, char* endTime, BOOL trueS);						///	用于，查询当前在线人数界定，当查询历史时间的时候，我们只需要界定查询当天的..

	BYTE* GetData(void* data);															///	获取指定数据;

	void CryptEncryptPackge(BYTE isCrypt, BYTE*& data, UINT16 headSize, UINT16 sourceLen, char* enccrptKey, UINT16* crptLen);	///数据包加密;

	typedef int (CGMProcessDataMan::*FuncProcessDataHandler)(void* data, int dataLen, CCMSession* session, DWORD sreq);
	std::map<UINT16, FuncProcessDataHandler> m_processFuncHandler;
public:
	std::map<UINT16, FuncProcessDataHandler>& GetProcessDataHandler()
	{
		return m_processFuncHandler;
	}
};
#endif