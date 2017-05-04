#include <set>
#include <list>
#include <vector>
#include "cyaTcp.h"
#include "cyaLocker.h"
#include "cyaTaskPool.h"
#include "cyaCore.h"
#include "ssmempool.h"
#include "cyaMemPool.h"

#define MAX_PROC_MSG_PERLOOP 256
#define MAX_CLOSE_DELAY_TICK 10	//毫秒
#define SESSION_KEEPALIVE_CHECK_TIMER 1000	//毫秒

#define USE_MEM_POOL	1

#if USE_MEM_POOL
static inline TSmallMemPoolObj& GetTCPSmallMemPoolObj__(INT_PTR itemKBytes = 16)
{
	BEGIN_LOCAL_SAFE_STATIC_OBJ_EX(TSmallMemPoolObj, xtcpMemPoolObj, (itemKBytes));
	END_LOCAL_SAFE_STATIC_OBJ(xtcpMemPoolObj);
}
#endif

///服务管理
class CyaTcp_ServeMan : public ICyaTcpServeMan
{
private:
	CyaTcp_ServeMan(const CyaTcp_ServeMan&);
	CyaTcp_ServeMan& operator = (const CyaTcp_ServeMan&);

public:
	CyaTcp_ServeMan(ICyaTcpSessionMaker* sessionMaker, UINT32 maxSessions, UINT32 sessionKeepAlive);
	~CyaTcp_ServeMan();

	virtual int  StartServe(const IPPADDR& serveIPP, BOOL reuse = true, int listenNum = 1024);
	virtual void StopServe(BOOL wait = true);

	virtual void SetSessionKeepAliveTime(UINT32 sec);
	virtual UINT32 GetSessionKeepAliveTime() const;

	virtual void SetMaxSessionsPerIoThread(int maxSess = 1024);
	virtual int  GetMaxSessionsPerIoThread() const;
	virtual UINT32  GetSessionsCount() const;

	virtual int  SendSessionData(ICyaTcpSession* session, const void* data, int len, BOOL canDiscard = false, BOOL needCopy = true, void* context = NULL);
	virtual int  SendSessionDataVec(ICyaTcpSession* session, const SockBufVec* bufs, int bufsCount, BOOL canDiscard = false, void* context = NULL);
	virtual int  SendSessionEmergencyData(ICyaTcpSession* session, const void* data, int len, BOOL needCopy = true, void* context = NULL);
	virtual int  SendSessionEmergencyDataVec(ICyaTcpSession* session, const SockBufVec* bufs, int bufsCount, void* context = NULL);

	virtual void SetSessionSendBufSize(ICyaTcpSession* session, int nBytes);
	virtual int  GetSessionSendBufSize(ICyaTcpSession* session);
	virtual int  GetSessionBufNotSentSize(ICyaTcpSession* session);

	virtual void TrustSession(SOCKET sock, void* param = NULL, BOOL wait = false);
	virtual int  CloseSession(ICyaTcpSession* session, BOOL wait = false);
	virtual void UpdateSessionKeepAliveTick(ICyaTcpSession* session);
	virtual void EnableSessionKeepAliveCheck(ICyaTcpSession* session, BOOL enable);
	virtual void DeleteThis();

private:
	static int THAcceptWorker(void* p);
	int OnAccept();
	int OnAfterAccepted(SOCKET acceptSock, const IPPADDR& peerIPP, void* param = NULL, COSSema* sema = NULL);

private:
	enum IoOpt
	{
		IO_READ,
		IO_WRITE,
		IO_ERROR
	};
	enum ServeManMsgType
	{
		mg_SessionBroken,
		mg_StartServe,
		mg_StopServe,
		mg_TrustSession
	};

	struct IoSessionOverlapped : public OVERLAPPED
	{
		IoOpt io;
		IoSessionOverlapped()
		{
			io = IO_ERROR;
		}
	};
	// 会话收发数据相关类型
	struct RecvBuf
	{
		void*	m_buf;
		int		m_needLen;
		int		m_receivedLen;
		ICyaTcpSession::IoReadType m_iort;
		BOOL	m_needFree;
		BOOL	m_notifyByTaskPool;

	private:
		void Init()
		{
			m_buf = NULL;
			m_needLen = 0;
			m_receivedLen = 0;
			m_iort = ICyaTcpSession::io_maxRead;
			m_needFree = false;
			m_notifyByTaskPool = false;
		}

	public:
		RecvBuf()
		{
			Init();
		}
		BOOL IsNullBuf() const
		{
			return NULL == m_buf;
		}
		void Close()
		{
			if (IsNullBuf())
				return;
			if (m_needFree)
				CyaTcp_Free(m_buf);
			Init();
		}
		~RecvBuf()
		{
			Close();
		}
	};
	struct SendBuf
	{
		void* m_buf;
		int	  m_needLen;
		int   m_sentLen;
		BOOL  m_needFree;
		void* m_context;

		SendBuf()
		{
			Init();
		}

		BOOL IsNull()
		{
			return m_buf == NULL;
		}

		void Init()
		{
			m_buf = NULL;
			m_needLen = 0;
			m_sentLen = 0;
			m_needFree = true;
			m_context = NULL;
		}

		void Close()
		{
			if (!IsNull() && m_needFree)
				CyaTcp_Free(m_buf);
			Init();
		}
	};

	struct SessionSite;
	struct PollSite		//处理IO完成事件类
	{
		PollSite(CyaTcp_ServeMan* serveMan);
		~PollSite();

		BOOL Open();
		void Close();

		static int THPollWorker(void* p);
		int OnPollWorker();

		int  GetSessionCount();
		void OnSessionBroken(CyaTcp_ServeMan::SessionSite* sessSite, int cause);
		BOOL AssociateSockOnIocp(SOCKET sock, UINT_PTR completeKey);
		void CheckSessionKeepAliveTime();

		HANDLE m_iocp;
		BOOL   m_exit;
		DWORD m_lastKeepAliveCheckTick;
		OSThreadSite m_pollThread;
		CyaTcp_ServeMan* m_serveMan;

		enum PollSiteMsgType
		{
			mg_AddSession,
			mg_DelSession,
			mg_AsyncReceivedCalled,
			mg_SendData
		};
		struct PollSiteMsg
		{
			PollSiteMsgType msg;
			union
			{
				struct
				{
					COSSema* sema;
					UINT_PTR completeKey;
				} addSession;

				struct
				{
					COSSema* sema;
					CyaTcp_ServeMan::SessionSite* sessSite;
				} delSession;

				struct
				{
					CyaTcp_ServeMan::SessionSite* sessSite;
				} ioWrite;

				struct
				{
					int onReceivedRet;
					CyaTcp_ServeMan::SessionSite* sessSite;
				} asyncReceivedCalled;

				struct
				{
					void* buf;
					int needLen;
					BOOL needFree;
					void* context;
					BOOL isEmergency;
					CyaTcp_ServeMan::SessionSite* sessSite;
				} sendData;
			};
		};

		CXTLocker m_msgQLocker;
		typedef std::list<PollSiteMsg> PollSiteMsgQ;
		PollSiteMsgQ m_msgQ;
		std::set<CyaTcp_ServeMan::SessionSite*> m_pollSessionSiteSet;

		BOOL ProcessPollSiteMsg();
		void OnMsg(const PollSiteMsg& msg);
		void PostMsg(const PollSiteMsg& msg);
		void PostEmergencyMsg(const PollSiteMsg& msg);

		void OnAddSession(UINT_PTR completeKey, COSSema* sema);
		void OnWriteData(CyaTcp_ServeMan::SessionSite* sessSite);
		void OnDelSession(CyaTcp_ServeMan::SessionSite* sessSite, COSSema* sema);
		void OnAsyncReceivedCalled(int onReceivedRet, CyaTcp_ServeMan::SessionSite* sessSite);
		void OnSendData(CyaTcp_ServeMan::SessionSite* sessSite, const void* data, int len, BOOL needCopy, void* context, BOOL isEmergency);
	};

	struct SessionSite : public ITBaseTask
	{
		SessionSite(/*CyaTcp_ServeMan* serveMan*/);
		~SessionSite();

		void Init();
		void Close();
		void Broken();

		void ConnectedSend();
		int  RequestReceive();
		int  OnReceived(DWORD len);

		int  OnSent(DWORD len);
		int  OnSendData(const void* data, int len, BOOL needFree, void* context, BOOL isEmergency = false);
		BOOL IsSendBufferFull();

		virtual void OnPoolTask();

		SOCKET	m_sessionSock;
		RecvBuf m_recvBuf;
		SendBuf m_sendBuf;
		PollSite* m_ownerPollSite;
		BOOL m_asyncReceivedCalling;

		IoSessionOverlapped m_inOv;
		IoSessionOverlapped m_outOv;
		CyaTcp_ServeMan*   m_serveMan;

		BOOL m_closeFlag;
		BOOL m_prepareClose;

		typedef std::list<SendBuf> SendBufQ;
		SendBufQ m_sendBufQ;	//数据发送队列

		int m_maxBufSize;
		int m_notSentSize;
		DWORD m_sessionTick;
		BOOL  m_enableKeepAliveCheck;

		CyaTcpSessionTmpl* m_session;
		char m_sessionBuf[sizeof(INT_PTR)];
	};

	long m_totalSessions;
	int	 m_maxSessionsPerThd;

	BOOL m_exit;
	SOCKET	m_bindSock;
	HANDLE	m_evt;
	OSThread m_thAcceptHandle;
	OSThreadID	m_thAcceptId;

	typedef std::vector<PollSite*> PollSiteVec;
	PollSiteVec m_pollSiteObjs;

	typedef std::set<SessionSite*> SessionSiteSet;
	SessionSiteSet m_sessionSiteSet;

	typedef std::list<SessionSite*> GarbageSessionSiteList;
	GarbageSessionSiteList m_grabageSessionList;

	typedef CMemPoolObj<SessionSite> SessionPool;
	UINT32 m_maxSessions;
	UINT32 m_sessionKeepAlive;
	SessionPool* m_sessionPool;
	ICyaTcpSessionMaker* m_sessionMaker;

private:
	struct ServeManMsg
	{
		ServeManMsgType msg;
		union
		{
			struct
			{
				CyaTcp_ServeMan::SessionSite* sessSite;
				int brokenCause;
			} brokenSession;

			struct
			{
				DWORD serveIp;
				int servePort;
				BOOL resue;
				int listenNum;
				COSSema* sema;
				int* ret;
			} startServe;

			struct
			{
				COSSema* sema;
			} stopServe;

			struct
			{
				SOCKET sock;
				void*  param;
				COSSema* sema;
			} trustParam;
		};
	};
	typedef std::list<ServeManMsg> ServeManMsgQ;
	ServeManMsgQ m_msgQ;
	CXTLocker m_msgLocker;

	BOOL OnProcessServeManMsg();
	void OnServeManMsg(const ServeManMsg& msg);
	void OnPostServeManMsg(const ServeManMsg& msg);

	BOOL OnRecyleGabageSessions();
	int  OnSessionBroken(CyaTcp_ServeMan::SessionSite* sessSite, int cause);
	int  OnStartServe(DWORD serveIp, int servePort, BOOL reuse, int listenNum, COSSema* sema, int* ret);
	void OnStopServe(COSSema* sema);
	void OnTrustSession(SOCKET sock, void* param, COSSema* sema);
};

CyaTcp_ServeMan::CyaTcp_ServeMan(ICyaTcpSessionMaker* sessionMaker, UINT32 maxSessions, UINT32 sessionKeepAlive)
								: m_sessionMaker(sessionMaker)
								, m_bindSock(INVALID_SOCKET)
								, m_evt(NULL)
								, m_thAcceptHandle(INVALID_OSTHREAD)
								, m_thAcceptId(INVALID_OSTHREADID)
								, m_exit(false)
								, m_maxSessionsPerThd(1024)
								, m_totalSessions(0)
								, m_sessionPool(NULL)
								, m_maxSessions(maxSessions)	
								, m_sessionKeepAlive(sessionKeepAlive)
{
	SockInit();
	OSCreateThread(&m_thAcceptHandle, &m_thAcceptId, THAcceptWorker, this, 0);
}

CyaTcp_ServeMan::~CyaTcp_ServeMan()
{
	//停止服务
	StopServe();

	//退出accept线程
	m_exit = true;
	if (m_thAcceptHandle != INVALID_OSTHREAD)
	{
		OSCloseThread(m_thAcceptHandle);
		m_thAcceptHandle = INVALID_OSTHREAD;
	}
}

void CyaTcp_ServeMan::DeleteThis()
{
	delete this;
}

int CyaTcp_ServeMan::StartServe(const IPPADDR& serveIPP, BOOL reuse/* = true*/, int listenNum/* = 1024*/)
{
	if (m_thAcceptId == OSThreadSelf())
		return OnStartServe(serveIPP.ip_, serveIPP.port_, reuse, listenNum, NULL, NULL);
	else
	{
		COSSema sema;
		int retCode = CYATCP_OK;

		ServeManMsg msg;
		msg.msg = mg_StartServe;
		msg.startServe.serveIp = serveIPP.ip_;
		msg.startServe.servePort = serveIPP.port_;
		msg.startServe.listenNum = listenNum;
		msg.startServe.resue = reuse;
		msg.startServe.sema = &sema;
		msg.startServe.ret = &retCode;
		OnPostServeManMsg(msg);
		sema.Wait();
	}
	return CYATCP_OK;
}

void CyaTcp_ServeMan::StopServe(BOOL wait /*= true*/)
{
	if (OSThreadSelf() == m_thAcceptId)
		OnStopServe(NULL);
	else
	{
		ServeManMsg msg;
		msg.msg = mg_StopServe;
		msg.stopServe.sema = NULL;
		if (wait)
		{
			COSSema sema;
			msg.stopServe.sema = &sema;
			OnPostServeManMsg(msg);
			sema.Wait();
		}
		else
			OnPostServeManMsg(msg);
	}
}

void CyaTcp_ServeMan::OnStopServe(COSSema* sema)
{
	//关闭会话
	SessionSiteSet::iterator it = m_sessionSiteSet.begin();
	for (; it != m_sessionSiteSet.end(); ++it)
	{
		SessionSite* sessSite = *it;
		CloseSession(sessSite->m_session, true);
	}

	//关闭accept操作
	if (m_evt)
	{
		CloseHandle(m_evt);
		m_evt = NULL;
	}
	if (SockValid(m_bindSock))
	{
		SockClose(m_bindSock);
		m_bindSock = INVALID_SOCKET;
	}

	//析构PollSite对象
	for (int i = 0; i < (int)m_pollSiteObjs.size(); ++i)
	{
		PollSite* pollSite = m_pollSiteObjs[i];
		pollSite->Close();
		delete pollSite;
	}
	m_pollSiteObjs.clear();

	//析构session对象
	it = m_sessionSiteSet.begin();
	for (; it != m_sessionSiteSet.end(); ++it)
	{
		SessionSite* sessSite = *it;
		int nCount = MAX_CLOSE_DELAY_TICK;
		while (sessSite->m_asyncReceivedCalling && nCount--)
			Sleep(1);
		sessSite->m_session->OnClosed(CYATCP_SELF_ACTIVE_CLOSED);
		m_sessionPool->DeleteObj(sessSite);
	}
	m_sessionSiteSet.clear();

	//清理垃圾会话
	OnRecyleGabageSessions();

	//销毁session内存池
	if (m_sessionPool != NULL)
	{
		delete m_sessionPool;
		m_sessionPool = NULL;
	}

	if (sema != NULL)
		sema->Post();
}

void CyaTcp_ServeMan::SetMaxSessionsPerIoThread(int maxSess/* = 1024*/)
{
	m_maxSessionsPerThd = max(maxSess, 1024);
}

int  CyaTcp_ServeMan::GetMaxSessionsPerIoThread() const
{
	return m_maxSessionsPerThd;
}

UINT32 CyaTcp_ServeMan::GetSessionsCount() const
{
	return (UINT32)m_sessionSiteSet.size();
}

void CyaTcp_ServeMan::SetSessionKeepAliveTime(UINT32 sec)
{
	m_sessionKeepAlive = sec;
}

UINT32 CyaTcp_ServeMan::GetSessionKeepAliveTime() const
{
	return m_sessionKeepAlive;
}

void CyaTcp_ServeMan::SetSessionSendBufSize(ICyaTcpSession* session, int nBytes)
{
	if (session == NULL || nBytes <= 0)
	{
		ASSERT(false);
		return;
	}
	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	sessSite->m_maxBufSize = nBytes;
}

int CyaTcp_ServeMan::GetSessionSendBufSize(ICyaTcpSession* session)
{
	if (session == NULL)
	{
		ASSERT(false);
		return 0;
	}

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	return sessSite->m_maxBufSize;
}

int CyaTcp_ServeMan::GetSessionBufNotSentSize(ICyaTcpSession* session)
{
	if (session == NULL)
	{
		ASSERT(false);
		return 0;
	}

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	return sessSite->m_notSentSize;
}

int CyaTcp_ServeMan::SendSessionData(ICyaTcpSession* session, const void* data, int len, BOOL canDiscard/* = true*/, BOOL needCopy/* = true*/, void* context/* = NULL*/)
{
	if (session == NULL || data == NULL || len <= 0)
		return CYATCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	if (sessSite->m_ownerPollSite == NULL)
		return CYATCP_ERROR_USE;
	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return CYATCP_SESSION_CLOSING;

	if (canDiscard && sessSite->IsSendBufferFull())
		return CYATCP_SENDBUF_IS_FULL;

	BOOL isPollThread = (OSThreadSelf() == sessSite->m_ownerPollSite->m_pollThread.m_id);
	CyaTcp_ServeMan::PollSite::PollSiteMsg msg;
	if (needCopy)
	{
		void* p = CyaTcp_Alloc(len);
		ASSERT(p != NULL);
		memcpy(p, data, len);
		msg.sendData.buf = p;
	}
	else
		msg.sendData.buf = (void*)data;

	msg.sendData.context = context;
	msg.sendData.needFree = needCopy;
	msg.sendData.needLen = len;
	msg.sendData.isEmergency = false;
	if (isPollThread)
	{
		sessSite->OnSendData(msg.sendData.buf, msg.sendData.needLen, msg.sendData.needFree, msg.sendData.context);
	}
	else
	{
		msg.sendData.sessSite = sessSite;
		msg.msg = CyaTcp_ServeMan::PollSite::mg_SendData;
		sessSite->m_ownerPollSite->PostMsg(msg);
	}
	return CYATCP_OK;
}

int  CyaTcp_ServeMan::SendSessionDataVec(ICyaTcpSession* session, const SockBufVec* bufs, int bufsCount, BOOL canDiscard/* = true*/, void* context/* = NULL*/)
{
	if (session == NULL || bufs == NULL || bufsCount <= 0)
		return CYATCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	if (sessSite->m_ownerPollSite == NULL)
		return CYATCP_ERROR_USE;

	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return CYATCP_SESSION_CLOSING;

	if (canDiscard && sessSite->IsSendBufferFull())
		return CYATCP_SENDBUF_IS_FULL;

	int nBytes = 0;
	for (int i = 0; i < bufsCount; ++i)
	{
		if (bufs[i].buf != NULL && bufs[i].bufBytes > 0)
			nBytes += bufs[i].bufBytes;
	}

	if (nBytes <= 0)
		return CYATCP_INVALID_PARAM;

	BYTE* p = (BYTE*)CyaTcp_Alloc(nBytes);
	ASSERT(p != NULL);
	int offset = 0;
	for (int i = 0; i < bufsCount; ++i)
	{
		if (bufs[i].buf != NULL && bufs[i].bufBytes > 0)
		{
			memcpy(p + offset, bufs[i].buf, bufs[i].bufBytes);
			offset += bufs[i].bufBytes;
		}
	}

	BOOL isPollThread = (OSThreadSelf() == sessSite->m_ownerPollSite->m_pollThread.m_id);
	CyaTcp_ServeMan::PollSite::PollSiteMsg msg;
	msg.sendData.context = context;
	msg.sendData.needFree = true;
	msg.sendData.buf = p;
	msg.sendData.needLen = nBytes;
	msg.sendData.isEmergency = false;
	if (isPollThread)
	{
		sessSite->OnSendData(msg.sendData.buf, msg.sendData.needLen, msg.sendData.needFree, msg.sendData.context);
	}
	else
	{
		msg.sendData.sessSite = sessSite;
		msg.msg = CyaTcp_ServeMan::PollSite::mg_SendData;
		sessSite->m_ownerPollSite->PostMsg(msg);
	}
	return CYATCP_OK;
}

int  CyaTcp_ServeMan::SendSessionEmergencyData(ICyaTcpSession* session, const void* data, int len, BOOL needCopy /*= true*/, void* context /*= NULL*/)
{
	if(session == NULL || data == NULL || len <= 0)
		return CYATCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	if(sessSite->m_ownerPollSite == NULL)
		return CYATCP_ERROR_USE;
	if(sessSite->m_closeFlag || sessSite->m_prepareClose)
		return CYATCP_SESSION_CLOSING;

	BOOL isPollThread = (OSThreadSelf() == sessSite->m_ownerPollSite->m_pollThread.m_id);
	CyaTcp_ServeMan::PollSite::PollSiteMsg msg;
	if(needCopy)
	{
		void* p = CyaTcp_Alloc(len);
		ASSERT(p != NULL);
		memcpy(p, data, len);
		msg.sendData.buf = p;
	}
	else
		msg.sendData.buf = (void*)data;

	msg.sendData.context = context;
	msg.sendData.needFree = needCopy;
	msg.sendData.needLen = len;
	msg.sendData.isEmergency = true;
	if(isPollThread)
	{
		sessSite->OnSendData(msg.sendData.buf, msg.sendData.needLen, msg.sendData.needFree, msg.sendData.context, true);
	}
	else
	{
		msg.sendData.sessSite = sessSite;
		msg.msg = CyaTcp_ServeMan::PollSite::mg_SendData;
		sessSite->m_ownerPollSite->PostEmergencyMsg(msg);
	}
	return CYATCP_OK;
}

int  CyaTcp_ServeMan::SendSessionEmergencyDataVec(ICyaTcpSession* session, const SockBufVec* bufs, int bufsCount, void* context/* = NULL*/)
{
	if (session == NULL || bufs == NULL || bufsCount <= 0)
		return CYATCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	if (sessSite->m_ownerPollSite == NULL)
		return CYATCP_ERROR_USE;

	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return CYATCP_SESSION_CLOSING;

	int nBytes = 0;
	for (int i = 0; i < bufsCount; ++i)
	{
		if (bufs[i].buf != NULL && bufs[i].bufBytes > 0)
			nBytes += bufs[i].bufBytes;
	}

	if (nBytes <= 0)
		return CYATCP_INVALID_PARAM;

	BYTE* p = (BYTE*)CyaTcp_Alloc(nBytes);
	ASSERT(p != NULL);
	int offset = 0;
	for (int i = 0; i < bufsCount; ++i)
	{
		if (bufs[i].buf != NULL && bufs[i].bufBytes > 0)
		{
			memcpy(p + offset, bufs[i].buf, bufs[i].bufBytes);
			offset += bufs[i].bufBytes;
		}
	}

	BOOL isPollThread = (OSThreadSelf() == sessSite->m_ownerPollSite->m_pollThread.m_id);
	CyaTcp_ServeMan::PollSite::PollSiteMsg msg;
	msg.sendData.context = context;
	msg.sendData.needFree = true;
	msg.sendData.buf = p;
	msg.sendData.needLen = nBytes;
	msg.sendData.isEmergency = true;
	if (isPollThread)
	{
		sessSite->OnSendData(msg.sendData.buf, msg.sendData.needLen, msg.sendData.needFree, msg.sendData.context, true);
	}
	else
	{
		msg.sendData.sessSite = sessSite;
		msg.msg = CyaTcp_ServeMan::PollSite::mg_SendData;
		sessSite->m_ownerPollSite->PostEmergencyMsg(msg);
	}
	return CYATCP_OK;
}

void CyaTcp_ServeMan::TrustSession(SOCKET sock, void* param /*= NULL*/, BOOL wait /*= false*/)
{
	if (!SockValid(sock))
		return;

	if (OSThreadSelf() == m_thAcceptId)
		OnTrustSession(sock, param, NULL);
	else
	{
		ServeManMsg msg;
		msg.msg = mg_TrustSession;
		msg.trustParam.sock = sock;
		msg.trustParam.param = param;
		msg.trustParam.sema = NULL;
		if (wait)
		{
			COSSema sema;
			msg.trustParam.sema = &sema;
			OnPostServeManMsg(msg);
			sema.Wait();
		}
		else
			OnPostServeManMsg(msg);
	}
}

int CyaTcp_ServeMan::CloseSession(ICyaTcpSession* session, BOOL wait/* = false*/)
{
	if (session == NULL)
		return CYATCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	ASSERT(sessSite->m_ownerPollSite);
	if (sessSite->m_ownerPollSite == NULL)
		return CYATCP_ERROR_USE;

	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return CYATCP_SESSION_CLOSING;

	sessSite->m_prepareClose = true;
	BOOL isPollThread = OSThreadSelf() == sessSite->m_ownerPollSite->m_pollThread.m_id;
	if (isPollThread)
	{
		sessSite->m_ownerPollSite->OnDelSession(sessSite, NULL);
	}
	else
	{
		PollSite::PollSiteMsg msg;
		msg.msg = PollSite::mg_DelSession;
		msg.delSession.sessSite = sessSite;
		msg.delSession.sema = NULL;

		if (wait)
		{
			COSSema sema;
			msg.delSession.sema = &sema;
			sessSite->m_ownerPollSite->PostMsg(msg);
			sema.Wait();
		}
		else
			sessSite->m_ownerPollSite->PostMsg(msg);
	}

	return CYATCP_OK;
}

void CyaTcp_ServeMan::UpdateSessionKeepAliveTick(ICyaTcpSession* session)
{
	if (session == NULL)
	{
		ASSERT(false);
		return;
	}
	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	sessSite->m_sessionTick = GetTickCount();
}

void CyaTcp_ServeMan::EnableSessionKeepAliveCheck(ICyaTcpSession* session, BOOL enable)
{
	if (session == NULL)
	{
		ASSERT(false);
		return;
	}
	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	sessSite->m_enableKeepAliveCheck = enable;
}

int CyaTcp_ServeMan::THAcceptWorker(void* p)
{
	CyaTcp_ServeMan* pThis = (CyaTcp_ServeMan*)p;
	ASSERT(pThis != NULL);
	return pThis->OnAccept();
}

int CyaTcp_ServeMan::OnAccept()
{
	while (!m_exit)
	{
		BOOL idle = false;

		//处理消息
		idle = OnProcessServeManMsg();

		//等待连接事件
		if (m_evt != NULL)
		{
			DWORD ret = WaitForSingleObject(m_evt, 0);
			if (ret == WAIT_OBJECT_0)
			{
				idle = false;
				WSANETWORKEVENTS evts;
				int r = WSAEnumNetworkEvents(m_bindSock, m_evt, &evts);
				if (r == 0 && (evts.lNetworkEvents & FD_ACCEPT))
				{
					IPPADDR peerIPP;
					SOCKET acceptSock = TCPAccept(m_bindSock, &peerIPP.ip_, &peerIPP.port_);
					if (SockValid(acceptSock))
					{
						int r = OnAfterAccepted(acceptSock, peerIPP);
						if (r != CYATCP_OK)
							SockClose(acceptSock);
					}
				}
			}
		}

		//回收垃圾session
		BOOL ret = OnRecyleGabageSessions();
		if (idle)	//若以上处理都为idle
			idle = ret;

		//若所有处理为idle，暂停一下
		if (idle)
			Sleep(1);
	}
	return 0;
}

int CyaTcp_ServeMan::OnAfterAccepted(SOCKET acceptSock, const IPPADDR& peerIPP, void* param /*= NULL*/, COSSema* sema/* = NULL*/)
{
	if (m_totalSessions >= m_maxSessions)	//超过了允许的最大会话数
	{
		printf("超过了服务最大会话数限制[%d]\n", m_maxSessions);
		return CYATCP_INNER_ERROR;
	}

	//设置会话属性
	SockSetNonBlock(acceptSock, true);
	SockSetTcpNoDelay(acceptSock, true);
	//SockSetLinger(acceptSock, true, 2000);

	//从sessionPool中分配一个会话Site
	if (m_sessionPool == NULL)
	{
		int sessionSize = m_sessionMaker->GetSessionObjSize();
		if (sessionSize <= 0)
			return CYATCP_INVALID_SESSION_SIZE;
		int unitSize = sizeof(SessionSite) + sessionSize - sizeof(INT_PTR);
		m_sessionPool = new SessionPool(unitSize, m_maxSessions, 64, 0);
		ASSERT(m_sessionPool != NULL);
	}

	//构建session
	SessionSite* sessSite = m_sessionPool->NewObj();
	sessSite->m_serveMan = this;
	m_sessionMaker->MakeSessionObj(sessSite->m_session);
	sessSite->m_session->SetServeMan(this);
	sessSite->m_sessionSock = acceptSock;
	InterlockedIncrement(&m_totalSessions);
	sessSite->m_session->OnConnected(acceptSock, peerIPP, m_totalSessions, param);

	// 分配session的读写poll任务
	PollSite* pollSite = NULL;
	for (int i = 0; i < (int)m_pollSiteObjs.size(); ++i)
	{
		PollSite* p = m_pollSiteObjs[i];
		if (p->GetSessionCount() < m_maxSessionsPerThd)
		{
			pollSite = p;
			break;
		}
	}

	if (NULL == pollSite)
	{
		pollSite = new CyaTcp_ServeMan::PollSite(this);
		ASSERT(pollSite != NULL);
		pollSite->Open();
		m_pollSiteObjs.push_back(pollSite);
	}

	ASSERT(m_sessionSiteSet.find(sessSite) == m_sessionSiteSet.end());
	m_sessionSiteSet.insert(sessSite);

	PollSite::PollSiteMsg msg;
	msg.msg = PollSite::mg_AddSession;
	msg.addSession.sema = sema;
	msg.addSession.completeKey = (UINT_PTR)sessSite;
	pollSite->PostMsg(msg);
	sessSite->m_ownerPollSite = pollSite;

	return CYATCP_OK;
}

BOOL CyaTcp_ServeMan::OnProcessServeManMsg()
{
	BOOL idle = true;
	if (m_msgQ.empty())
		return idle;

#if 0
	ServeManMsgQ msgQ;
	{
		CXTAutoLock lock(m_msgLocker);
		if (m_msgQ.empty())
			return idle;
		msgQ.assign(m_msgQ.begin(), m_msgQ.end());
		m_msgQ.clear();
	}
	for (ServeManMsgQ::iterator it = msgQ.begin(); it != msgQ.end(); ++it)
	{
		idle = false;
		OnServeManMsg(*it);
	}
#else
	ServeManMsgQ msgQ;
	{
		CXTAutoLock lock(m_msgLocker);
		if (m_msgQ.empty())
			return true;
		if (MAX_PROC_MSG_PERLOOP > 0)
		{
			int nLoop = 0;
			ServeManMsgQ::iterator it = m_msgQ.begin();
			while (it != m_msgQ.end() && nLoop < MAX_PROC_MSG_PERLOOP)
			{
				msgQ.push_back(*it);
				++it;
				++nLoop;
			}
			m_msgQ.erase(m_msgQ.begin(), it);
		}
		else
		{
			msgQ.assign(m_msgQ.begin(), m_msgQ.end());
			m_msgQ.clear();
		}
	}

	idle = false;
	for (ServeManMsgQ::iterator it = msgQ.begin(); it != msgQ.end(); ++it)
		OnServeManMsg(*it);
#endif
	return idle;
}

void CyaTcp_ServeMan::OnServeManMsg(const ServeManMsg& msg)
{
	switch (msg.msg)
	{
	case mg_SessionBroken:
	{
		int r = OnSessionBroken(msg.brokenSession.sessSite, msg.brokenSession.brokenCause);
		if (r != CYATCP_OK)
			OnPostServeManMsg(msg);
	}
	break;
	case mg_StartServe:
		OnStartServe(msg.startServe.serveIp, msg.startServe.servePort, msg.startServe.resue, msg.startServe.listenNum, msg.startServe.sema, msg.startServe.ret);
		break;
	case mg_StopServe:
		OnStopServe(msg.stopServe.sema);
		break;
	case mg_TrustSession:
		OnTrustSession(msg.trustParam.sock, msg.trustParam.param, msg.trustParam.sema);
		break;
	default:
		break;
	}
}

BOOL CyaTcp_ServeMan::OnRecyleGabageSessions()
{
	if (m_grabageSessionList.empty())
		return true;

	GarbageSessionSiteList::iterator it = m_grabageSessionList.begin();
	for (; it != m_grabageSessionList.end(); ++it)
	{
		SessionSite* site = *it;
		ASSERT(site->m_closeFlag);
		m_sessionPool->DeleteObj(site);
	}
	m_grabageSessionList.clear();
	return false;
}

int  CyaTcp_ServeMan::OnStartServe(DWORD serveIp, int servePort, BOOL reuse, int listenNum, COSSema* sema, int* ret)
{
	ASSERT(m_sessionMaker != NULL);
	int retCode = CYATCP_OK;
	if (SockValid(m_bindSock))	//服务已经启动了
		retCode = CYATCP_SERVE_STARTED;

	if (ret != NULL)
		*ret = retCode;

	if (retCode != CYATCP_OK)
	{
		if (sema)
			sema->Post();
		return retCode;
	}

	if (m_sessionPool == NULL)
	{
		int sessionSize = m_sessionMaker->GetSessionObjSize();
		if (sessionSize <= 0)
			retCode = CYATCP_INVALID_SESSION_SIZE;
		else
		{
			int unitSize = sizeof(SessionSite) + sessionSize - sizeof(INT_PTR);
			m_sessionPool = new SessionPool(unitSize, m_maxSessions, 64, 0);
			ASSERT(m_sessionPool != NULL);
		}
	}

	if (ret != NULL)
		*ret = retCode;

	if (retCode != CYATCP_OK)
	{
		if (sema)
			sema->Post();
		return retCode;
	}

	SOCKET sock = TCPNewServerEx(serveIp, servePort, reuse, listenNum);
	if (!SockValid(sock))
	{
		DWORD const winErr = GetLastError();
		if (WSAEADDRINUSE == winErr)
		{
			printf("Only one usage of each tcp serve, port: %d", servePort);
			retCode = CYATCP_SYSTEM_ERROR;
		}
		else if (WSAEMFILE == winErr || WSAENOBUFS == winErr)
		{
			printf("Not enough system resource for the creating, port: %s", servePort);
			retCode = CYATCP_SYSTEM_ERROR;
		}
		else if (WSAEADDRNOTAVAIL == winErr)
		{
			printf("The requested address is not valid in its context, port: %d", servePort);
			retCode = CYATCP_SYSTEM_ERROR;
		}
		else
		{
			printf("System call error for the creating, port: %d", servePort);
			retCode = CYATCP_SYSTEM_ERROR;
		}
	}

	if (ret != NULL)
		*ret = retCode;

	if (retCode != CYATCP_OK)
	{
		if (sema)
			sema->Post();
		return retCode;
	}

	SockSetNonBlock(sock, true);
	m_evt = CreateEvent(NULL, false, false, NULL);
	ASSERT(NULL != m_evt);
	WSAEventSelect(sock, m_evt, FD_ACCEPT | FD_CLOSE);
	m_bindSock = sock;
	if (sema)
		sema->Post();
	return retCode;
}

void CyaTcp_ServeMan::OnTrustSession(SOCKET sock, void* param, COSSema* sema)
{
	IPPADDR peerIPP = SockGetPeer(sock);
	int r = OnAfterAccepted(sock, peerIPP, param, sema);
	if (r != 0 && sema != NULL)
		sema->Post();
}

int CyaTcp_ServeMan::OnSessionBroken(CyaTcp_ServeMan::SessionSite* sessSite, int cause)
{
	SessionSiteSet::iterator it = m_sessionSiteSet.find(sessSite);
	if (it == m_sessionSiteSet.end())
	{
		//ASSERT(false);	//在StopServe停止服务的时候有可能发生
		return CYATCP_OK;
	}

	if (sessSite->m_asyncReceivedCalling)	//还处于异步回调中不能回收
		return CYATCP_SESSION_USING;

	//减少总会话计数
	InterlockedDecrement(&m_totalSessions);

#if defined(_DEBUG)
	if (sessSite->m_prepareClose)
		ASSERT(sessSite->m_closeFlag);
	else if (sessSite->m_closeFlag)
		ASSERT(!sessSite->m_prepareClose);
#endif

	m_sessionSiteSet.erase(it);
	sessSite->m_session->OnClosed(cause);

	//加入到垃圾会话表中
	m_grabageSessionList.push_back(sessSite);
	return CYATCP_OK;
}

void CyaTcp_ServeMan::OnPostServeManMsg(const ServeManMsg& msg)
{
	CXTAutoLock lock(m_msgLocker);
	m_msgQ.push_back(msg);
}

CyaTcp_ServeMan::PollSite::PollSite(CyaTcp_ServeMan* serveMan)
									: m_serveMan(serveMan)
									, m_exit(false)
									, m_iocp(NULL)
									, m_lastKeepAliveCheckTick(0)
{

}

CyaTcp_ServeMan::PollSite::~PollSite()
{

}

BOOL CyaTcp_ServeMan::PollSite::Open()
{
	m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_iocp == NULL)
		return false;
	return m_pollThread.Open(THPollWorker, this, 0);
}

void CyaTcp_ServeMan::PollSite::Close()
{
	m_exit = true;
	m_pollThread.Close();
	if (m_iocp)
	{
		CloseHandle(m_iocp);
		m_iocp = NULL;
	}
}

int CyaTcp_ServeMan::PollSite::GetSessionCount() /*const*/
{
	return (int)m_pollSessionSiteSet.size()/*m_sessionCount*/;
}

int CyaTcp_ServeMan::PollSite::THPollWorker(void* p)
{
	PollSite* pThis = (PollSite*)p;
	ASSERT(pThis != NULL);
	return pThis->OnPollWorker();
}

int CyaTcp_ServeMan::PollSite::OnPollWorker()
{
	while (!m_exit)
	{
		//空闲标记
		BOOL idle = false;

		//是否端线
		BOOL isBroken = false;

		//先处理消息
		idle = ProcessPollSiteMsg();

		DWORD dwTransfer = 0;
		ULONG_PTR completeKey = 0;
		CyaTcp_ServeMan::SessionSite* sessSite = NULL;
		CyaTcp_ServeMan::IoSessionOverlapped* ov;
		BOOL ret = GetQueuedCompletionStatus(m_iocp, &dwTransfer, (PULONG_PTR)&completeKey, (LPOVERLAPPED*)&ov, 0);
		if (!ret)
		{
			DWORD err = WSAGetLastError();
			if (WAIT_TIMEOUT == err)	//超时
			{
				//检查会话存活时间
				CheckSessionKeepAliveTime();

				if (idle)
					Sleep(1);
				continue;
			}

			isBroken = true;
		}
		else
		{
			if (dwTransfer == 0)
				isBroken = true;
		}

		idle = false;

		sessSite = (CyaTcp_ServeMan::SessionSite*)completeKey;
		ASSERT(sessSite != NULL);
		if (isBroken)
		{
			//断线
			OnSessionBroken(sessSite, CYATCP_PEER_CLOSED);
			//检查会话存活时间
			CheckSessionKeepAliveTime();
			continue;
		}

		int readRet = CYATCP_OK;
		int writeRet = CYATCP_OK;
		if (m_pollSessionSiteSet.find(sessSite) == m_pollSessionSiteSet.end())
			continue;

		if (ov->io == CyaTcp_ServeMan::IO_READ)
			readRet = sessSite->OnReceived(dwTransfer);
		else if (ov->io == CyaTcp_ServeMan::IO_WRITE)
			writeRet = sessSite->OnSent(dwTransfer);
		else
			ASSERT(false);

		if (readRet != CYATCP_OK || writeRet != CYATCP_OK)	//将连接断掉
			OnSessionBroken(sessSite, readRet == CYATCP_OK ? writeRet : readRet);

		//检查会话存活时间
		CheckSessionKeepAliveTime();

		if (idle)
			Sleep(1);
	}
	return 0;
}

void CyaTcp_ServeMan::PollSite::OnSessionBroken(CyaTcp_ServeMan::SessionSite* sessSite, int cause)
{
	//ASSERT(OSThreadSelf() == m_pollThread.m_id);
	if (m_pollSessionSiteSet.find(sessSite) == m_pollSessionSiteSet.end())
		return;

	//sessSite->Close();
	sessSite->Broken();
	m_pollSessionSiteSet.erase(sessSite);

	//投递断线消息到ServeMan线程中
	CyaTcp_ServeMan::ServeManMsg msg;
	msg.msg = CyaTcp_ServeMan::mg_SessionBroken;
	msg.brokenSession.sessSite = sessSite;
	msg.brokenSession.brokenCause = cause;
	m_serveMan->OnPostServeManMsg(msg);
}

BOOL CyaTcp_ServeMan::PollSite::AssociateSockOnIocp(SOCKET sock, UINT_PTR completeKey)
{
	ASSERT(m_iocp != NULL);
	return CreateIoCompletionPort((HANDLE)sock, m_iocp, completeKey, 0) == NULL ? false : true;
}

void CyaTcp_ServeMan::PollSite::CheckSessionKeepAliveTime()
{
	UINT32 keepAlive = m_serveMan->GetSessionKeepAliveTime() * 1000;	//毫秒
	if (keepAlive <= 0)
		return;

	DWORD nowTick = GetTickCount();
	DWORD interval = nowTick - m_lastKeepAliveCheckTick;
	if (interval < SESSION_KEEPALIVE_CHECK_TIMER)
		return;

	m_lastKeepAliveCheckTick = nowTick;
	std::set<CyaTcp_ServeMan::SessionSite*>::iterator it = m_pollSessionSiteSet.begin();
	for (; it != m_pollSessionSiteSet.end();)
	{
		CyaTcp_ServeMan::SessionSite* sessSite = *it;
		ASSERT(sessSite != NULL);
		DWORD sessionTick = sessSite->m_sessionTick;
		if (!sessSite->m_enableKeepAliveCheck || sessionTick == 0)
		{
			++it;
			continue;
		}

		int interval = nowTick - sessionTick;
		if (interval < 0)
		{
			sessSite->m_sessionTick = nowTick;
			++it;
			continue;
		}

		if (interval > keepAlive)
		{
			sessSite->m_closeFlag = true;
			m_pollSessionSiteSet.erase(it++);
			sessSite->Broken();

			//投递断线消息到ServeMan线程中
			CyaTcp_ServeMan::ServeManMsg msg;
			msg.msg = CyaTcp_ServeMan::mg_SessionBroken;
			msg.brokenSession.sessSite = sessSite;
			msg.brokenSession.brokenCause = CYATCP_KEEPALIVE_EXPIRED;
			m_serveMan->OnPostServeManMsg(msg);
		}
		else
			++it;
	}
}

BOOL CyaTcp_ServeMan::PollSite::ProcessPollSiteMsg()
{
	if (m_msgQ.empty())
		return true;

#if 0
	PollSiteMsgQ msgQ;
	{
		CXTAutoLock lock(m_msgQLocker);
		if (m_msgQ.empty())
			return true;
		msgQ.assign(m_msgQ.begin(), m_msgQ.end());
		m_msgQ.clear();
	}

	for (PollSiteMsgQ::iterator it = msgQ.begin(); it != msgQ.end(); ++it)
		OnMsg(*it);
#else
	PollSiteMsgQ msgQ;
	{
		CXTAutoLock lock(m_msgQLocker);
		if (m_msgQ.empty())
			return true;
		if (MAX_PROC_MSG_PERLOOP > 0)
		{
			int nLoop = 0;
			PollSiteMsgQ::iterator it = m_msgQ.begin();
			while (it != m_msgQ.end() && nLoop < MAX_PROC_MSG_PERLOOP)
			{
				msgQ.push_back(*it);
				++it;
				++nLoop;
			}
			m_msgQ.erase(m_msgQ.begin(), it);
		}
		else
		{
			msgQ.assign(m_msgQ.begin(), m_msgQ.end());
			m_msgQ.clear();
		}
	}

	for (PollSiteMsgQ::iterator it = msgQ.begin(); it != msgQ.end(); ++it)
		OnMsg(*it);
#endif
	return false;
}

void CyaTcp_ServeMan::PollSite::PostMsg(const PollSiteMsg& msg)
{
	CXTAutoLock lock(m_msgQLocker);
	m_msgQ.push_back(msg);
}

void CyaTcp_ServeMan::PollSite::PostEmergencyMsg(const PollSiteMsg& msg)
{
	CXTAutoLock lock(m_msgQLocker);
	m_msgQ.push_front(msg);
}

void CyaTcp_ServeMan::PollSite::OnMsg(const PollSiteMsg& msg)
{
	switch (msg.msg)
	{
	case mg_AddSession:
		OnAddSession(msg.addSession.completeKey, msg.addSession.sema);
		break;
	case mg_DelSession:
		OnDelSession(msg.delSession.sessSite, msg.delSession.sema);
		break;
	case mg_AsyncReceivedCalled:
		OnAsyncReceivedCalled(msg.asyncReceivedCalled.onReceivedRet, msg.asyncReceivedCalled.sessSite);
		break;
	case mg_SendData:
		OnSendData(msg.sendData.sessSite, msg.sendData.buf, msg.sendData.needLen, msg.sendData.needFree, msg.sendData.context, msg.sendData.isEmergency);
		break;
	default:
		break;
	}
}

void CyaTcp_ServeMan::PollSite::OnAddSession(UINT_PTR completeKey, COSSema* sema/* = NULL*/)
{
	//ASSERT(OSThreadSelf() == m_pollThread.m_id);
	CyaTcp_ServeMan::SessionSite* sessSite = (CyaTcp_ServeMan::SessionSite*)completeKey;
	ASSERT(m_pollSessionSiteSet.find(sessSite) == m_pollSessionSiteSet.end());
	ASSERT(SockValid(sessSite->m_sessionSock));

	BOOL ret = false;
	sessSite->m_sessionTick = GetTickCount();
	m_pollSessionSiteSet.insert(sessSite);

	ret = AssociateSockOnIocp(sessSite->m_sessionSock, completeKey);
	ASSERT(ret);

	sessSite->ConnectedSend();
	int r = sessSite->RequestReceive();
	if (sema != NULL)
		sema->Post();

	if (r != 0)
		OnSessionBroken(sessSite, r);
}

void CyaTcp_ServeMan::PollSite::OnDelSession(CyaTcp_ServeMan::SessionSite* sessSite, COSSema* sema /*= NULL*/)
{
	//ASSERT(OSThreadSelf() == m_pollThread.m_id);
	// 	if(m_pollSessionSiteSet.find(sessSite) == m_pollSessionSiteSet.end())
	// 		return ;

	// 	if(SockValid(sessSite->m_sessionSock))
	// 	{
	// 		SockClose(sessSite->m_sessionSock);
	// 		sessSite->m_sessionSock = INVALID_SOCKET;
	// 	}
	OnSessionBroken(sessSite, CYATCP_SELF_ACTIVE_CLOSED);

	if (sema != NULL)
		sema->Post();
}

void CyaTcp_ServeMan::PollSite::OnSendData(CyaTcp_ServeMan::SessionSite* sessSite, const void* data, int len, BOOL needCopy, void* context, BOOL isEmergency)
{
	//ASSERT(OSThreadSelf() == m_pollThread.m_id);
	if (m_pollSessionSiteSet.find(sessSite) != m_pollSessionSiteSet.end())
		sessSite->OnSendData(data, len, needCopy, context, isEmergency);
	else
	{
		if (needCopy)
			CyaTcp_Free((void*)data);
		else
			ASSERT(false);
	}
}

void CyaTcp_ServeMan::PollSite::OnAsyncReceivedCalled(int onReceivedRet, CyaTcp_ServeMan::SessionSite* sessSite)
{
	ASSERT(OSThreadSelf() == m_pollThread.m_id);
	ASSERT(sessSite->m_asyncReceivedCalling);
	ASSERT(sessSite->m_recvBuf.m_needLen >= sessSite->m_recvBuf.m_receivedLen);

	if (sessSite->m_prepareClose || sessSite->m_closeFlag)
	{
		sessSite->m_asyncReceivedCalling = false;
		return;
	}

	sessSite->m_asyncReceivedCalling = false;
	sessSite->m_recvBuf.Close();
	if (onReceivedRet != CYATCP_OK) ///将连接断掉
		OnSessionBroken(sessSite, onReceivedRet);
	else
	{
		int r = sessSite->RequestReceive();
		if (r != CYATCP_OK)	///将连接断掉
			OnSessionBroken(sessSite, r);
	}
}

CyaTcp_ServeMan::SessionSite::SessionSite(/*CyaTcp_ServeMan* serveMan*/)
										: m_session((CyaTcpSessionTmpl*)m_sessionBuf)
	//, _serveMan(serveMan)
{
	Init();
}

CyaTcp_ServeMan::SessionSite::~SessionSite()
{
	Close();
	Init();
	m_session->~CyaTcpSessionTmpl();
}

void CyaTcp_ServeMan::SessionSite::Init()
{
	m_closeFlag = false;
	m_prepareClose = false;
	m_ownerPollSite = NULL;
	m_sessionSock = INVALID_SOCKET;
	m_maxBufSize = 0;
	m_notSentSize = 0;
	m_asyncReceivedCalling = false;
	memset(&m_inOv, 0, sizeof(m_inOv));
	memset(&m_outOv, 0, sizeof(m_outOv));
	m_inOv.io = IO_ERROR;
	m_sessionTick = 0;
	m_enableKeepAliveCheck = true;
	ASSERT((void*)m_session == (void*)m_sessionBuf);
}

void CyaTcp_ServeMan::SessionSite::Close()
{
	if (SockValid(m_sessionSock))
	{
		SockClose(m_sessionSock);
		m_sessionSock = INVALID_SOCKET;
	}
	m_recvBuf.Close();

	if (!m_sendBuf.IsNull())
		m_session->OnSent(m_sendBuf.m_buf, m_sendBuf.m_needLen, m_sendBuf.m_needFree, m_sendBuf.m_context, false);
	m_sendBuf.Close();

	for (SendBufQ::iterator it = m_sendBufQ.begin(); it != m_sendBufQ.end(); ++it)
	{
		SendBuf& buf = *it;
		if (!buf.IsNull())
			m_session->OnSent(buf.m_buf, buf.m_needLen, buf.m_needFree, buf.m_context, false);
		buf.Close();
	}
	m_sendBufQ.clear();
	m_closeFlag = true;
}

void CyaTcp_ServeMan::SessionSite::Broken()
{
	if (SockValid(m_sessionSock))
	{
		SockClose(m_sessionSock);
		m_sessionSock = INVALID_SOCKET;
	}

	// 	if(!m_sendBuf.IsNull())
	// 		m_session->OnSent(m_sendBuf.m_buf, m_sendBuf.m_needLen, m_sendBuf.m_needFree, m_sendBuf.m_context, false);

	for (SendBufQ::iterator it = m_sendBufQ.begin(); it != m_sendBufQ.end(); ++it)
	{
		SendBuf& buf = *it;
		if (!buf.IsNull())
			m_session->OnSent(buf.m_buf, buf.m_needLen, buf.m_needFree, buf.m_context, false);
		buf.Close();
	}
	m_sendBufQ.clear();
	m_closeFlag = true;
}

BOOL CyaTcp_ServeMan::SessionSite::IsSendBufferFull()
{
	if (m_maxBufSize <= 0)
		return false;
	return  m_notSentSize >= m_maxBufSize ? true : false;
}

void CyaTcp_ServeMan::SessionSite::ConnectedSend()
{
	ASSERT(m_session != NULL);
	if (m_session != NULL)
		m_session->OnConnectedSend();
}

int CyaTcp_ServeMan::SessionSite::RequestReceive()
{
	ASSERT(m_recvBuf.IsNullBuf());
	ASSERT(!m_asyncReceivedCalling);
	void* buf = NULL;
	int len = 0;
	BOOL asynNotify = false;
	ICyaTcpSession::IoReadType rt = ICyaTcpSession::io_maxRead;
	int r = m_session->RequestReceive(buf, len, rt, asynNotify);
	if (r != CYATCP_OK)
		return r;
	if (len <= 0)
		return CYATCP_INVALID_READ_SIZE;

	if (buf == NULL)
	{
		buf = CyaTcp_Alloc(len);
		ASSERT(buf != NULL);
		m_recvBuf.m_needFree = true;
	}
	else
		m_recvBuf.m_needFree = false;

	m_recvBuf.m_buf = buf;
	m_recvBuf.m_needLen = len;
	m_recvBuf.m_receivedLen = 0;
	m_recvBuf.m_notifyByTaskPool = asynNotify;
	m_recvBuf.m_iort = rt;

	//投递一次读IO
	WSABUF bufs;
	bufs.buf = (char*)buf;
	bufs.len = len;
	DWORD dwRead = 0;
	DWORD flag = 0;
	m_inOv.io = CyaTcp_ServeMan::IO_READ;
	r = WSARecv(m_sessionSock, &bufs, 1, &dwRead, &flag, (LPOVERLAPPED)&m_inOv, NULL);
	if (r != 0)
	{
		DWORD err = WSAGetLastError();
		if (r == WSAENOTSOCK)
			r = CYATCP_SESSION_BROKEN;
		else if (err == WSA_IO_PENDING || err == WSAEWOULDBLOCK || err == WSAENOBUFS)
			r = CYATCP_OK;
		else
			r = CYATCP_SESSION_BROKEN;
		// 		ASSERT(err == WSA_IO_PENDING || err == WSAECONNABORTED || err == WSAECONNRESET || err == WSAENOTSOCK);
		// 		if(r == WSAENOTSOCK || WSAENOTSOCK == err || err == WSAECONNABORTED || err == WSAECONNRESET)
		// 			return ROSETCP_SESSION_BROKEN;
	}
	return r;
}

int CyaTcp_ServeMan::SessionSite::OnReceived(DWORD len)
{
	ASSERT(len > 0);
	int r = CYATCP_OK;
	BOOL canCallOnReceived = false;
	m_recvBuf.m_receivedLen += len;
	if (ICyaTcpSession::io_maxRead == m_recvBuf.m_iort || m_recvBuf.m_receivedLen == m_recvBuf.m_needLen)
		canCallOnReceived = true;

	if (!canCallOnReceived)
	{
		WSABUF bufs;
		bufs.buf = ((char*)m_recvBuf.m_buf) + m_recvBuf.m_receivedLen;
		bufs.len = m_recvBuf.m_needLen - m_recvBuf.m_receivedLen;
		ASSERT(bufs.len > 0);
		DWORD dwRead = 0;
		DWORD flag = 0;
		m_inOv.io = CyaTcp_ServeMan::IO_READ;
		int ret = WSARecv(m_sessionSock, &bufs, 1, &dwRead, &flag, (LPOVERLAPPED)&m_inOv, NULL);
		if (ret != 0)
		{
			DWORD err = WSAGetLastError();
			if (ret == WSAENOTSOCK)
				r = CYATCP_SESSION_BROKEN;
			else if (err == WSA_IO_PENDING || err == WSAEWOULDBLOCK || err == WSAENOBUFS)
				r = CYATCP_OK;
			else
				r = CYATCP_SESSION_BROKEN;

			// 			ASSERT(err == WSA_IO_PENDING || err == WSAEWOULDBLOCK || err == WSAECONNRESET || err == WSAENOTSOCK);
			// 			if(ret == WSAENOTSOCK || err == WSAENOTSOCK || err == WSAECONNRESET || err == WSAECONNABORTED)
			// 				r = ROSETCP_SESSION_BROKEN;
			// 			else
			// 				r = ROSETCP_OK;
		}
	}
	else
	{
		BOOL continueRecv = true;
		if (m_recvBuf.m_notifyByTaskPool)
		{
			m_asyncReceivedCalling = true;
			CyaCore_FetchTaskPool()->Push(this);
		}
		else
		{
			r = m_session->OnReceived(m_recvBuf.m_buf, m_recvBuf.m_receivedLen, false, continueRecv);
			m_recvBuf.Close();
			if (r != CYATCP_OK)
				return r;
			r = RequestReceive();
		}
	}
	return r;
}

int CyaTcp_ServeMan::SessionSite::OnSendData(const void* data, int len, BOOL needFree, void* context, BOOL isEmergency /*= false*/)
{
	if (data != NULL && len > 0)
	{
		SendBuf sendBuf;
		sendBuf.m_buf = (void*)data;
		sendBuf.m_needLen = len;
		sendBuf.m_needFree = needFree;
		sendBuf.m_context = context;
		if (isEmergency)
			m_sendBufQ.push_front(sendBuf);
		else
			m_sendBufQ.push_back(sendBuf);
		m_notSentSize += len;
	}

	if (!m_sendBuf.IsNull())
		return CYATCP_OK;
	if (m_sendBufQ.empty())
		return CYATCP_OK;

	m_sendBuf = *m_sendBufQ.begin();
	m_sendBufQ.erase(m_sendBufQ.begin());

	//投递一次写IO
	WSABUF bufs;
	DWORD dwSend = 0;
	bufs.buf = (char*)m_sendBuf.m_buf + m_sendBuf.m_sentLen;
	bufs.len = m_sendBuf.m_needLen - m_sendBuf.m_sentLen;
	m_outOv.io = CyaTcp_ServeMan::IO_WRITE;
	int r = WSASend(m_sessionSock, &bufs, 1, &dwSend, 0, (LPOVERLAPPED)&m_outOv, NULL);
	if (r != 0)
	{
		DWORD err = WSAGetLastError();
		if (r == WSAENOTSOCK)
			r = CYATCP_SESSION_BROKEN;
		else if (err == WSA_IO_PENDING || err == WSAEWOULDBLOCK || err == WSAENOBUFS)
			r = CYATCP_OK;
		else
			r = CYATCP_SESSION_BROKEN;
		// 		ASSERT(err == WSA_IO_PENDING || err == WSAECONNABORTED || err == WSAECONNRESET || err == WSAENOTSOCK);
		// 		if(r == WSAENOTSOCK || err == WSAENOTSOCK || err == WSAECONNABORTED || err == WSAECONNRESET)
		// 			return ROSETCP_SESSION_BROKEN;
	}
	return r;
}

int CyaTcp_ServeMan::SessionSite::OnSent(DWORD len)
{
	ASSERT(len > 0);
	int r = 0;
	m_sendBuf.m_sentLen += len;
	if (m_sendBuf.m_sentLen == m_sendBuf.m_needLen)
	{
		m_notSentSize -= m_sendBuf.m_needLen;
		ASSERT(m_notSentSize >= 0);
		m_session->OnSent(m_sendBuf.m_buf, m_sendBuf.m_sentLen, m_sendBuf.m_needFree, m_sendBuf.m_context, true);
		m_sendBuf.Close();
		r = OnSendData(NULL, 0, false, NULL);
	}
	else
	{
		//投递一次写IO
		WSABUF bufs;
		DWORD dwSend = 0;
		bufs.buf = (char*)m_sendBuf.m_buf + m_sendBuf.m_sentLen;
		bufs.len = m_sendBuf.m_needLen - m_sendBuf.m_sentLen;
		ASSERT(bufs.len > 0);
		m_outOv.io = CyaTcp_ServeMan::IO_WRITE;
		int r = WSASend(m_sessionSock, &bufs, 1, &dwSend, 0, (LPOVERLAPPED)&m_outOv, NULL);
		if (r != 0)
		{
			DWORD err = WSAGetLastError();
			if (r == WSAENOTSOCK)
				r = CYATCP_SESSION_BROKEN;
			else if (err == WSA_IO_PENDING || err == WSAEWOULDBLOCK || err == WSAENOBUFS)
				r = CYATCP_OK;
			else
				r = CYATCP_SESSION_BROKEN;
			// 			ASSERT(err == WSA_IO_PENDING || err == WSAECONNABORTED || err == WSAECONNRESET || err == WSAENOTSOCK);
			// 			if(r == WSAENOTSOCK || err == WSAENOTSOCK || err == WSAECONNABORTED || err == WSAECONNRESET)
			// 				r = ROSETCP_SESSION_BROKEN;
			// 			else
			// 				r = ROSETCP_OK;
		}
	}
	return r;
}

void CyaTcp_ServeMan::SessionSite::OnPoolTask()
{
	ASSERT(m_asyncReceivedCalling);
	BOOL continueRecv = true;
	int r = m_session->OnReceived(m_recvBuf.m_buf, m_recvBuf.m_receivedLen, true, continueRecv);
	if (m_prepareClose || m_closeFlag)
	{
		m_asyncReceivedCalling = false;
		return;
	}

	CyaTcp_ServeMan::PollSite::PollSiteMsg msg;
	msg.msg = CyaTcp_ServeMan::PollSite::mg_AsyncReceivedCalled;
	msg.asyncReceivedCalled.onReceivedRet = r;
	msg.asyncReceivedCalled.sessSite = this;
	m_ownerPollSite->PostMsg(msg);
}

CYATCP_API int CyaTcp_MakeServeMan(ICyaTcpServeMan*& serveMan,
									ICyaTcpSessionMaker* sessionMaker,
									UINT32 maxSessions/* = 64*1024*/,
									UINT32 sessionKeepAlive/* = 0*/)
{
	CyaTcp_ServeMan* man = new CyaTcp_ServeMan(sessionMaker, maxSessions, sessionKeepAlive);
	if (man == NULL)
		return CYATCP_ALLOC_MEM_FAILED;
	serveMan = man;
	return CYATCP_OK;
}

///分配内存
CYATCP_API void* CyaTcp_Alloc(INT_PTR si)
{
#if USE_MEM_POOL
	return GetTCPSmallMemPoolObj__().m_poolObj->Alloc(si);
#else
	return ::malloc(si);
#endif
}

///释放内存
CYATCP_API void CyaTcp_Free(void* p)
{
	if (p == NULL)
		return;
#if USE_MEM_POOL
	GetTCPSmallMemPoolObj__().m_poolObj->Free(p);
#else
	::free(p);
#endif
}