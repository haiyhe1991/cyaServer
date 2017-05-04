#ifndef __CYA_TCP_H__
#define __CYA_TCP_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaSock.h"
#include "cyaTcpExports.h"
#include "cyaTcpErr.h"

///�Ự������
struct ICyaTcpSessionMaker
{
	virtual ~ICyaTcpSessionMaker(){}

	///��ȡsession��С,��������ʵ��{return sizeof(XXXSession);}
	virtual int GetSessionObjSize() = 0;

	///����session����,��������ʵ��{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session) = 0;
};

///�Ự
struct ICyaTcpSession
{
	enum IoReadType
	{
		io_mustRead,
		io_maxRead
	};
	virtual ~ICyaTcpSession(){}

	///@sock[IN]	: ����������SOCKET
	///@peerIPP[IN] : �Զ�ip��ַ��Ϣ
	///@sessionCount[IN] : �Ự����
	///@param[IN] : �������йܻỰ�ӿ�ʱ��paramΪTrustSession����Ĳ���
	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL) = 0;

	///һ����������Ҫ������������(���ڴ˵���ICyaTcpServeMan�еķ������ݽӿ�)
	virtual void OnConnectedSend() = 0;

	///���ӶϿ�
	///@cause[IN] : ���ӶϿ�ԭ��
	virtual void OnClosed(int cause) = 0;

	///@buf[OUT]    : ���ݱ�����Ļ�����(���������ɿ��Զ��������)
	///@bufLen[OUT] : ��Ҫ��ȡ���ݵĳ���(>0)
	///@rt[OUT]		: ��ȡ����(io_maxRead->ֻҪ�������ݾʹ���OnReceived, io_mustRead->�������bufLen�Ŵ���OnReceived)
	///@asynNotify[OUT] : true/false->ͬ��/�첽����OnReceived
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify) = 0;

	///@buf[IN] : ��ȡ���ݻ�����
	///@bufLen[IN] : ���ݳ���
	///@callbackByTask[IN] : ���÷�ʽ(ͬRequestReceive��asynNotify����ֵ)
	///@continueRecv[IN] : true/false->�Ƿ������ȡ
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv) = 0;

	///���ݷ���֪ͨ
	///@buf[IN] : �������ݵ�buffer
	///@bufLen[IN] : �������ݳ���
	///@isCopy[IN] : �����Ƿ񿽱���
	///@context[IN] : ���������Ĳ���
	///@isSendSuccess[IN] : �����Ƿ��ͳɹ�
	virtual void OnSent(const void* buf, int bufLen, BOOL isCopy, void* context, BOOL isSendSuccess) = 0;
};

//�Ự����
struct ICyaTcpServeMan
{
	virtual ~ICyaTcpServeMan(){}

	///��������
	///@serveIPP[IN] : ����ip��ַ��Ϣ
	///@reuse[IN] :	   �Ƿ����ü���SOCKET SO_REUSEADDR����
	///@listenNum[IN] : listen����
	virtual int  StartServe(const IPPADDR& serveIPP, BOOL reuse = true, int listenNum = 1024) = 0;

	///ֹͣ����
	virtual void StopServe(BOOL wait = true) = 0;

	///���÷�����session�Ĵ��ʱ��
	///@data[sec] : ���ʱ��(��λ:��, 0-����)
	virtual void SetSessionKeepAliveTime(UINT32 sec) = 0;

	///��ȡ������session�Ĵ��ʱ��(��λ:��, 0-����)
	virtual UINT32 GetSessionKeepAliveTime() const = 0;

	///���õ����̹߳���ĻỰ��
	///@maxSess[IN] : �����̹߳���ĻỰ��
	virtual void SetMaxSessionsPerIoThread(int maxSess = 1024) = 0;

	///��ȡ��ǰ�����̹߳���ĻỰ����
	virtual int  GetMaxSessionsPerIoThread() const = 0;

	///��ȡ��ǰ�Ự����
	virtual UINT32 GetSessionsCount() const = 0;

	///��������
	///@session[IN] : �Ựsession
	///@data[IN]    : Ҫ��������buffer��ַ
	///@len[IN]     : �������ݳ���
	///@canDiscard[IN] : �����ݰ��ܷ���
	///@needCopy[IN] : �Ƿ���Ҫ��������
	///@context[IN] : ���������Ĳ���(��OnSent����)
	virtual int  SendSessionData(ICyaTcpSession* session, const void* data, int len, BOOL canDiscard = true, BOOL needCopy = true, void* context = NULL) = 0;

	///��������(�ɷ��Ͷ��buffer,����Ҫ�����ڲ�һ�ο���)
	///@session[IN] : �Ựsession
	///@bufs[IN]    : SockBufVec����
	///@bufsCount[IN]     : SockBufVec����Ԫ�ظ���
	///@canDiscard[IN] : �����ݰ��ܷ���
	///@context[IN] : ���������Ĳ���(��OnSent����)
	virtual int  SendSessionDataVec(ICyaTcpSession* session, const SockBufVec* bufs, int bufsCount, BOOL canDiscard = true, void* context = NULL) = 0;

	///���ͽ�������(��߷������ȼ�,���ɱ�����,ע��:ʹ�øýӿڿ��ܳ��ֺ��͵������ȵ������,�����ʹ��)
	///@session[IN] : �Ựsession
	///@data[IN]    : Ҫ��������buffer��ַ
	///@len[IN]     : �������ݳ���
	///@needCopy[IN] : �Ƿ���Ҫ��������
	///@context[IN] : ���������Ĳ���(��OnSent����)
	virtual int  SendSessionEmergencyData(ICyaTcpSession* session, const void* data, int len, BOOL needCopy = true, void* context = NULL) = 0;

	///���ͽ�������(�ɷ��Ͷ��buffer,����Ҫ�����ڲ�һ�ο���,��߷������ȼ�,���ɱ�����,ע��:ʹ�øýӿڿ��ܳ��ֺ��͵������ȵ������,�����ʹ��)
	///@session[IN] : �Ựsession
	///@bufs[IN]    : SockBufVec����
	///@bufsCount[IN]     : SockBufVec����Ԫ�ظ���
	///@context[IN] : ���������Ĳ���(��OnSent����)
	virtual int  SendSessionEmergencyDataVec(ICyaTcpSession* session, const SockBufVec* bufs, int bufsCount, void* context = NULL) = 0;

	///���ûỰ���ͻ����С
	///@session[IN] : �Ựsession
	///@nBytes[IN] : �Ự���ͻ����������ֽ���(0-������)
	virtual void SetSessionSendBufSize(ICyaTcpSession* session, int nBytes) = 0;

	///��ȡ�Ự���ͻ����С(�ֽ�)
	///@session[IN] : �Ựsession
	virtual int GetSessionSendBufSize(ICyaTcpSession* session) = 0;

	///��ȡ�Ự��ǰ���ͻ�����δ�������ݴ�С(�ֽ�)
	///@session[IN] : �Ựsession
	virtual int GetSessionBufNotSentSize(ICyaTcpSession* session) = 0;

	///�йܻỰ
	///@sock[IN] : Ҫ�йܵ�SOCKET
	///@param[IN] : OnConnected������param
	///@wait[IN] : �Ƿ�ȴ�OnConnected����
	virtual void TrustSession(SOCKET sock, void* param = NULL, BOOL wait = false) = 0;

	///�رջỰ
	///@session[IN] : �Ựsession
	///@wait[IN] : �Ƿ�ȴ�socket�ر�
	virtual int  CloseSession(ICyaTcpSession* session, BOOL wait = false) = 0;

	//���»Ự���ʱ��
	///@session[IN] : �Ựsession
	virtual void UpdateSessionKeepAliveTick(ICyaTcpSession* session) = 0;

	//�Ƿ�ԸûỰ���ʱ����
	///@session[IN] : �Ựsession
	virtual void EnableSessionKeepAliveCheck(ICyaTcpSession* session, BOOL enable) = 0;

	///ɾ��ICyaTcpServeMan����
	virtual void DeleteThis() = 0;
};

//ʹ���߱����CyaTcpSessionTmpl�̳�
class CyaTcpSessionTmpl : public ICyaTcpSession
{
public:
	CyaTcpSessionTmpl()
		: m_serveMan(NULL)
	{
	}

	~CyaTcpSessionTmpl()
	{
	}

	virtual void OnConnectedSend()
	{

	}

	virtual void  OnSent(const void* buf, int bufLen, BOOL isCopy, void* context, BOOL isSendSuccess)
	{
		buf; bufLen; isCopy; context; isSendSuccess;
	}

	void SetServeMan(ICyaTcpServeMan* serveMan)
	{
		m_serveMan = serveMan;
	}

	ICyaTcpServeMan* GetServeMan()
	{
		return m_serveMan;
	}

private:
	ICyaTcpServeMan* m_serveMan;
};


///����IRoseTcpServeMan����
///@serveMan[OUT] : IRoseTcpServeMan�����ַ
///@sessionMaker[IN] : �Ự����������
///@maxSessions[IN] : �������֧�ֵĻỰ��
///@sessionKeepAlive[IN] : �����лỰ�Ĵ��ʱ��(��λ:��,0-����)
CYATCP_API int CyaTcp_MakeServeMan(ICyaTcpServeMan*& serveMan,
									ICyaTcpSessionMaker* sessionMaker,
									UINT32 maxSessions = 64 * 1024,
									UINT32 sessionKeepAlive = 0);

///�����ڴ�
CYATCP_API void* CyaTcp_Alloc(INT_PTR si);

///�ͷ��ڴ�
CYATCP_API void CyaTcp_Free(void* p);

//�����ͷŶ���
#define CyaTcp_New(className) \
	::new(CyaTcp_Alloc(sizeof(className))) className()

#define CyaTcp_NewEx(className, constructParam) \
	::new(CyaTcp_Alloc(sizeof(className))) className constructParam

template <typename classObj>
inline void CyaTcp_Delete(classObj* obj)
{
	if (obj == NULL)	
		return; 
	obj->~classObj(); 
	CyaTcp_Free(obj);
}

#endif