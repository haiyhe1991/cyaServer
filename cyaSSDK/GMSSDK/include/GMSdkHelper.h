#ifndef __GM_SDK_HELPER_H__
#define __GM_SDK_HELPER_H__

#include "cyaBase.h"	//�ļ�����;
#include "SemaDataMan.h"
#include "cyaLocker.h"	//ͬ������_�߳���;
#include "cyaLog.h"
#include "cyaCoreTimer.h"	//Timer
#include "GMProtocal.h"	//��ѯ�����������Ϣ�ṹ;
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
	///��ͳ��ʵʱ��������;
	TimeAddedinfo* QueryRealTimeAddStatistics(UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);

	//��ѯ��ֵ��������;
	RecharegeStatisticeIofo* QueryOrderTracking(UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);

	//��ѯ��ֵ����ͳ��;
	StartisticsInfo* QueryPecharGeStatistcs(UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);
	//��ѯ����ͳ��(���ա����ա����ա�����ʮ�ա���)
	GMQLeavekeepDateInfo* QueryLeaveKeepStatistics(UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endTime, BYTE lktype, BYTE lkTimeType, BYTE& retNum, UINT16* retCode, UINT32* total);
	//��ѯʵʱ����;
	OnlineInfo* QueryOnlineCurrent(UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* startTime, BYTE& retNum, UINT16* retCode, UINT32* total);
	//�ȼ��ֲ�ͳ��;
	GM_Grade_RatingStatisticsInfo* QueryGrade_RatingStatisticsInfo(UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);

public:
	void InitFunc();
	///��¼
	int Login();
	///�˳�
	void Loginout();
	///���ñ��ػỰ
	void SetLocalSession(GMSdkSesssion* session);
	///д���������
	void InputProcessData(UINT16 packType, void* data, int dataLen, DWORD req, BOOL isOver);
	///�ͷ�handle
	void  ReleaseHandle();
	///����session����
	void StartReconnect();

	///��������ʱ��
	void UpdateHeatbeatTime()
	{
		m_heartbeatTime = GetTickCount();
		LogInfo(("Client�������\n"));
	}

	///����Timer��������
	static void ReconnectTimer(void* param, TTimerID id);
	///����Timer��������
	static void HeartbeatTimer(void* param, TTimerID id);

private:

	long AddOneForRequence();
	BOOL IsNull(void* p)
	{
		if (p == NULL)
			return true;
		return false;
	}


	///��������
	///@cmdCode[IN]: ������
	///@req[IN]: �����к�
	///@buf[IN: ��������
	///@bufLen[IN]: �������ݳ���
	///isWait[IN]:�Ƿ�ȴ�
	///outData[OUT]: ��������
	///@outDataLen[OUT]: �������ݳ���
	int SendDataMan(UINT16 cmdCode, DWORD req, void* buf, int bufLen, BYTE isWait, void*& outData, UINT32* outDataLen);

	///���ݰ����ܼ����ͳ��ȴ���
	///@isCrypt[IN]: �Ƿ����
	///@buf[OUT]:��������
	///@unCrptfLen[IN]:ԭʼ���ݳ���
	///@sourceHead[IN]:����Э��ͷ����
	///@key[IN]: ����key
	///@crptLen[OUT]:�������ݳ���
	void EncryptPakcge(BYTE isCrypt, BYTE*& buf, UINT16 unCrptfLen, UINT16 sourceHead, char* key, UINT16* crptLen);

	///��������������
	void SendHeartPackge();
	///��������
	void OnReconnect();
	///�������
	void OnHeartbeat();

private:
	typedef std::map<UINT32, CSemaDataMan*> SemaData;
	typedef int (CGMSdkHelper::*FuncProcessManHandle)(UINT16 plat, UINT16 partId, const char* startTime, const char* endTime, int* getCount);
	std::map<BYTE, FuncProcessManHandle> m_funcHanlde;


	char m_gmsIp[17];			//���������IP
	UINT16 m_gmsPort;			//����������˿�
	UINT16 m_connTimeout;		//���ӳ�ʱ

	SemaData m_semaData;		//�����кż�
	CXTLocker m_locker;
	long m_requence;			//��ʼ�����к�
	char m_key[33];			//����
	BOOL m_isLogin;			//�Ƿ��¼

	GMSdkSesssion* m_pLocalSession; //���ػỰ
	CyaCoreTimer m_reconnectTimer; //����Timer
	CyaCoreTimer m_heartbeatTimer; //����Timer
	DWORD m_heartbeatTime; //����ʱ��
	DWORD m_heartbeatInterval; //�������ʱ��

	GMProtocalHead m_hearbeatHead;



};


#endif