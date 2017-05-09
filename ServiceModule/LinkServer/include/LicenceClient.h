#ifndef __LICENCE_CLIENT_H__
#define __LICENCE_CLIENT_H__

#include "LscSessionMaker.h"
#include "LscSession.h"
#include "cyaSync.h"
#include "cyaCoreTimer.h"

class LicenceClient
{
public:
	LicenceClient();
	~LicenceClient();

	enum LINK_ON_OFF_STATUS
	{
		LINK_STATUS_ON = 0,
		LINK_STATUS_OFF = 1,
		LINK_STATUS_UNKOWN = 2
	};

	/// ��������֤������������  dwConTimeOut:��ʱʱ�� bReConnect:�Ƿ����� dwInterval:�������
	int ConnectLicenceServer(DWORD dwConTimeOut = 5000, BOOL bReConnect = true, DWORD dwInterval = 1000);
	int ReConnectLicenceServer();
	/// ��¼��֤������
	int LogInLicenceServer(SOCKET sockLogin);
	/// ��ȡ����״̬
	int GetLinkStatus(BYTE& cLkStatus);
	/// ����֤�����������ɫ���� nUerID:�û�ID
	int ReportRoleCreated(UINT32 nUerID);
	/// ����֤�����������ɫɾ�� nUerID:�û�ID
	int ReportRoleDeleted(UINT32 nUerID);
	/// ֪ͨ�˻�����
	void ReportUserExit(UINT32 nUerID);
	/// ֪ͨ��֤�������û���½�ɹ�
	int ReportUserLogin(UINT32 nUerID);

	void SetLocalLSCSession(LSCSession* pSession);
	/// ����֪ͨ
	void LscSessionBroken();

	/// ����֤������������״̬
	bool LSClientStatus() const;
	/// ��ȡ����(�ϱ�������)ʱ����
	int GetReportTimer();

	/// ����֤�������������ݽӿ�
	int SendCtrlData(UINT16 cmdCode, UINT16 retCode, const void* payload, UINT16 payloadLen);

private:
	/// ��֤��������Ϣ�ص�
	static int LscClientMsgData(bool bEncrypt, UINT16 nPktType, void* pMsgData, int nMsgLen, void* context);

	/// ���ư����(DATA_TOKEN_KEY_PACKET)
	int OnKeyPacket(void* pMsgData, int nMsgLen);
	/// ���ݰ����(PAKCET_DATA)
	int OnDataPacket(void* pMsgData, int nMsgLen);
	/// ���ư����(DATA_CTRL_PACKET)
	int OnContrlPacket(void* pMsgData, int nMsgLen);

	/*  DATA_TOKEN_KEY_PACKET   */
	/// ��ȡ��Կ�ظ�
	int OnKeyConnectMsg(const void* pMsgData, int MsgLen);

	/*  PAKCET_DATA */
	/// ��¼��֤�������ظ�
	int OnDataLoginMsg(UINT16 nRetCode);
	/// ��ȡLinkServer״̬�ظ�
	int OnDataStatusMsg(UINT16 nRetCode, BYTE cLkStatus);
	/// ��ȡ��������ظ�
	int OnGetReportTimer(UINT16 nRetCode, UINT32 theTime);

	/*  DATA_CTRL_PACKET    */
	/// ��֤��������ȡToken
	int OnContrlGetToken(void* pMsgData, int nMsgLen);
	/// ��֤����������LinkServer
	int OnContrlStartLinker();
	/// ��֤������ֹͣLinkServer
	int OnContrlStopLinker();
	/// ���յ�������Ϣ
	int OnContrlKickUser(void* pMsgData, int nMsgLen);

	/// ����֤�������ϱ�������
	int OnConnectsMsg();


private:
	/// ������ʱ��
	static void ReconnectTimer(void* pParam, TTimerID id);
	void OnReconnect();

	/// �ϱ�����������ʱ��, Ĭ��Ϊ5���ϱ�һ�Σ�û�������仯������LinkServer�رգ����ñ�
	static void RePortConsTimer(void* pParam, TTimerID id);
	void OnRePortCons();

	/// ��Ϣ�ص�������
	int OnLscClientMsgData(bool bEncrypt, UINT16 nPktType, void* pMsgData, int nMsgLen);
	/// �ϱ���ɫɾ���򴴽�
	int ReportRoleChanged(UINT32 nUerID, BYTE cModel);
	/// �������ݵ���֤������
	int InputSessinData(const void* pSessionData, int nDataLen);

	BOOL WaitSema(DWORD dwWaitTimeOut = 5000);
	void PostSema();
private:
	LSCSessionMaker* m_pLscSessionMaker;
	ICyaTcpServeMan* m_pLscSessionMan;
	LSCSession* m_pLscSession;  /// ��Client��Ӧ��Session

	DWORD m_dwConTimeOut;       /// ������֤�������ĳ�ʱʱ��
	BOOL m_bReConEnable;        /// �Ƿ��������
	DWORD m_dwReConInterval;    /// ��������ʱ����
	BOOL m_bLoginOk;            /// �Ƿ��¼�ɹ�
	BYTE m_cLinkStatus;         /// LinkServer״̬

	CyaCoreTimer m_iReConTimer;    /// ��������Timer
	CyaCoreTimer m_iReportTimer;   /// �ϱ�������Timer
	int m_nLinkSesssionNum;         /// ����LinkServer��������û�б仯�Ͳ����ϱ�

	UINT32 m_nPayloadToken;     /// ͨ��Э��Token
	char m_szPayloadKey[DEFAULT_KEY_SIZE + 1];    /// ͨ��������Կ
	OSSema m_iSema; ///��Ϣ����Ϊ�첽�����ź���ͬ��
};

#endif