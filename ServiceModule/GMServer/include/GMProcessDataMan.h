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

	/*������Ϣ���ݴ���*/
	int OnMsgProcessDataMan(UINT16 cmdCode, void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---ʵʱ����ͳ��---*/
	int	QueryNewStatistics(void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---ʵʱ��ֵͳ��---*/
	///������ѯ;			
	int QueryRechargeOrder(void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---��ֵ����ͳ��---*/
	int QueryPecharGeStatistcs(void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---��������ͳ��---*/
	int QueryLeaveKeepStatistics(void* data, int dataLen, CCMSession* session, DWORD sreq);

	/*---��������ͳ��---*/
	int QueryRealtimeoneline(void* data, int datalen, CCMSession* session, DWORD sreq);

	/*---��������ͳ��---*/
	/*int QueryOnlineNumStatistics(void* data,int dataLen,CCMSession* session, DWORD sreq);*/

	/*---�ȼ��ֲ�ͳ��---*/
	int QueryLevelDistribution(void* data, int dataLen, CCMSession* session, DWORD sreq);

	//======================================================================================
private:

	int ProcessDataSend(CCMSession* session, void* data, int dataLen);										///		�������ݷ���;	

	void RechargeOrder(std::vector<RecharegeStatisticeIofo>& OrderInfo, RecharegeStatisticeIofo data[1]);	///		��ֵ������ϸ��Ϣ����;

	void RecharegeStat(std::vector<StartisticsInfo>& OrderInfo, StartisticsInfo data[1]);					///		��ֵͳ����󷵻�ȡֵ����;

private:
	/*�ڴ���䴦��*/
	void* AllocMan(int nSize)
	{
		return CyaTcp_Alloc(nSize + 1);
	}
	/*�ڴ��ͷŴ���*/
	void FreeMan(void* p)
	{
		if (p != NULL)
		{
			CyaTcp_Free(p);
			p = NULL;
		}
	}

	BOOL IfDayDateTime(const char* startTime);											///	�Ƿ��ǵ�ǰ���ڣ�����Ǿͷ���	return TRUE;return FALSE;

	int IfMinuteTime(char* startTime);													///	�Ƿ��ǵ�ǰʱ�䣬ʱ�룬����10����һ����Ϊ��;

	void Timetodefine(char* startTIme, char* endTime, BOOL trueS);						///	���ڣ���ѯ��ǰ���������綨������ѯ��ʷʱ���ʱ������ֻ��Ҫ�綨��ѯ�����..

	BYTE* GetData(void* data);															///	��ȡָ������;

	void CryptEncryptPackge(BYTE isCrypt, BYTE*& data, UINT16 headSize, UINT16 sourceLen, char* enccrptKey, UINT16* crptLen);	///���ݰ�����;

	typedef int (CGMProcessDataMan::*FuncProcessDataHandler)(void* data, int dataLen, CCMSession* session, DWORD sreq);
	std::map<UINT16, FuncProcessDataHandler> m_processFuncHandler;
public:
	std::map<UINT16, FuncProcessDataHandler>& GetProcessDataHandler()
	{
		return m_processFuncHandler;
	}
};
#endif