//有问题，没有看，直接copy过来的

#if defined(__shitandfuck__)//暂定不让其编译

#include <set>
#include <list>
#include <vector>
#include "rosetcp.h"
#include "roselocker.h"
#include "rosetaskpool.h"
#include "rose_max_min.h"
#include "rosecore.h"
#include "roseosopt.h"
#include "ssmempool.h"
#include "rosemempool.h"

#include <sys/poll.h>
#include <sys/epoll.h>

#define MAX_FD_PER_WAIT 64
static int const ERR_POLL_FLAG = POLLNVAL | POLLERR | POLLHUP
#ifdef POLLRDHUP
| POLLRDHUP
#endif
;

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
class RoseTcp_ServeMan : public IRoseTcpServeMan
{
private:
	RoseTcp_ServeMan(const RoseTcp_ServeMan&);
	RoseTcp_ServeMan& operator = (const RoseTcp_ServeMan&);

public:
	RoseTcp_ServeMan(IRoseTcpSessionMaker* sessionMaker, UINT32 maxSessions, UINT32 sessionKeepAlive);
	~RoseTcp_ServeMan();

	virtual int  StartServe(const IPPADDR& peerIPP, BOOL reuse = true, int listenNum = 1024);
	virtual void StopServe(BOOL wait = true);

	virtual void SetSessionKeepAliveTime(UINT32 sec);
	virtual UINT32 GetSessionKeepAliveTime() const;

	virtual void SetMaxSessionsPerIoThread(int maxSess = 1024);
	virtual int  GetMaxSessionsPerIoThread() const;
	virtual UINT32 GetSessionsCount() const;

	virtual int  SendSessionData(IRoseTcpSession* session, const void* data, int len, BOOL canDiscard = true, BOOL needCopy = true, void* context = NULL);
	virtual int  SendSessionDataVec(IRoseTcpSession* session, const SockBufVec* bufs, int bufsCount, BOOL canDiscard = true, void* context = NULL);
	virtual int  SendSessionEmergencyData(IRoseTcpSession* session, const void* data, int len, BOOL needCopy = true, void* context = NULL);
	virtual int  SendSessionEmergencyDataVec(IRoseTcpSession* session, const SockBufVec* bufs, int bufsCount, void* context = NULL);

	virtual void SetSessionSendBufSize(IRoseTcpSession* session, int nBytes);
	virtual int  GetSessionSendBufSize(IRoseTcpSession* session);
	virtual int  GetSessionBufNotSentSize(IRoseTcpSession* session);

	virtual void TrustSession(SOCKET sock, void* param = NULL, BOOL wait = false);
	virtual int  CloseSession(IRoseTcpSession* session, BOOL wait = false);
	virtual void UpdateSessionKeepAliveTick(IRoseTcpSession* session);
	virtual void EnableSessionKeepAliveCheck(IRoseTcpSession* session, BOOL enable);
	virtual void DeleteThis();

public:
	int GetMaxSessionsPerThread() const;

private:
	static int THAcceptWorker(void* p);
	int OnAccept();
	int OnAfterAccepted(SOCKET acceptSock, const IPPADDR& peerIPP, void* param = NULL, COSSema* sema = NULL);

private:
	enum ServeManMsgType
	{
		mg_SessionBroken,
		mg_StartServe,
		mg_StopServe,
		mg_TrustSession
	};

	// 会话收发数据相关类型
	struct RecvBuf
	{
		void*	m_buf;
		int		m_needLen;
		int		m_receivedLen;
		IRoseTcpSession::IoReadType m_iort;
		BOOL	m_needFree;
		BOOL	m_notifyByTaskPool;

	private:
		void Init()
		{
			m_buf = NULL;
			m_needLen = 0;
			m_receivedLen = 0;
			m_iort = IRoseTcpSession::io_maxRead;
			m_needFree = false;
			m_notifyByTaskPool = false;
		}

	public:
		RecvBuf()
		{
			Init();
		}
		BOOL IsNull() const
		{
			return NULL == m_buf;
		}
		void Close()
		{
			if (IsNull())
				return;
			if (m_needFree)
				RoseTcp_Free(m_buf);
			Init();
		}
		~RecvBuf()
		{
			Close();
		}
	};
	struct SendBuf
	{
		void*	m_buf;
		int		m_needLen;
		int		m_sentLen;
		BOOL	m_needFree;
		void*	m_context;

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
				RoseTcp_Free(m_buf);
			Init();
		}
	};

	struct SessionSite;
	struct PollSite		//处理IO完成事件类
	{
		PollSite(RoseTcp_ServeMan* serveMan);
		~PollSite();

		BOOL Open();
		void Close();

		static int THPollWorker(void* p);
		int OnPollWorker();

		int  GetPollSessionCount();
		void OnSessionBroken(RoseTcp_ServeMan::SessionSite* sessSite, int cause);
		BOOL OnProcessEvent(const struct epoll_event& epollEvt);
		void CheckSessionKeepAliveTime();

		BOOL m_exit;
		int	m_pollfd;
		DWORD m_lastKeepAliveCheckTick;
		OSThreadSite m_pollThread;
		RoseTcp_ServeMan* m_serveMan;
		struct epoll_event m_waitEvent[MAX_FD_PER_WAIT];

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
					RoseTcp_ServeMan::SessionSite* sessSite;
				} delSession;

				struct
				{
					RoseTcp_ServeMan::SessionSite* sessSite;
				} ioWrite;

				struct
				{
					int onReceivedRet;
					RoseTcp_ServeMan::SessionSite* sessSite;
				} asyncReceivedCalled;

				struct
				{
					void* buf;
					int needLen;
					BOOL needFree;
					void* context;
					BOOL isEmergency;
					RoseTcp_ServeMan::SessionSite* sessSite;
				} sendData;
			};
		};

		CXTLocker m_msgQLocker;
		typedef std::list<PollSiteMsg> PollSiteMsgQ;
		PollSiteMsgQ m_msgQ;
		std::set<RoseTcp_ServeMan::SessionSite*> m_pollSessionSiteSet;

		BOOL ProcessPollSiteMsg();
		void OnMsg(const PollSiteMsg& msg);
		void PostMsg(const PollSiteMsg& msg);
		void PostEmergencyMsg(const PollSiteMsg& msg);

		void OnAddSession(UINT_PTR completeKey, COSSema* sema);
		void OnDelSession(RoseTcp_ServeMan::SessionSite* sessSite, COSSema* sema);
		void OnAsyncReceivedCalled(int onReceivedRet, RoseTcp_ServeMan::SessionSite* sessSite);
		void OnSendData(RoseTcp_ServeMan::SessionSite* sessSite, const void* data, int len, BOOL needFree, void* context, BOOL isEmergency);
	};

	struct SessionSite : public ITBaseTask
	{
		SessionSite(/*RoseTcp_ServeMan* serveMan*/);
		~SessionSite();

		void Init();
		void Close();
		void Broken();

		int OnReadSingal();
		int OnWriteSingal(BOOL& idle);

		int  RequestReceive();
		void OnSendData(const void* data, int len, BOOL needFree, void* context, BOOL isEmergency = false);
		void ConnectedSend();
		BOOL IsSendBufferFull();

		virtual void OnPoolTask();

		SOCKET	m_sessionSock;
		RecvBuf m_recvBuf;
		SendBuf m_sendBuf;
		PollSite* m_ownerPollSite;
		BOOL	m_asyncReceivedCalling;
		RoseTcp_ServeMan* m_serveMan;
		BOOL	m_closeFlag;
		BOOL	m_prepareClose;

		typedef std::list<SendBuf> SendBufQ;
		SendBufQ m_sendBufQ;

		int m_maxBufSize;
		int m_notSentSize;
		DWORD m_sessionTick;
		BOOL  m_enableKeepAliveCheck;

		RoseTcpSessionTmpl* m_session;
		char m_sessionBuf[sizeof(INT_PTR)];
	};

	long m_totalSessions;
	int	m_maxSessionsPerThd;

	BOOL m_exit;
	SOCKET m_bindSock;
	OSThread m_thAcceptHandle;
	OSThreadID	m_thAcceptId;

	typedef std::vector<PollSite*> PollSiteVec;
	PollSiteVec m_pollSiteObjs;

	typedef std::set<SessionSite*> SessionSiteSet;
	SessionSiteSet m_sessionSiteSet;

	typedef std::list<SessionSite*> GarbageSessionSiteList;
	GarbageSessionSiteList m_grabageSessionList;

	int m_epoll;
	int m_needPollFdCount;
	struct epoll_event m_waitEvent[MAX_FD_PER_WAIT];

	typedef CMemPoolObj<SessionSite> SessionPool;
	SessionPool* m_sessionPool;
	UINT32 m_maxSessions;
	UINT32 m_sessionKeepAlive;
	IRoseTcpSessionMaker* m_sessionMaker;

private:
	struct ServeManMsg
	{
		ServeManMsgType msg;
		union
		{
			struct
			{
				RoseTcp_ServeMan::SessionSite* sessSite;
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
	void OnOnServeManMsg(const ServeManMsg& msg);
	void OnPostServeManMsg(const ServeManMsg& msg);

	BOOL OnRecyleGabageSessions();
	int  OnSessionBroken(RoseTcp_ServeMan::SessionSite* sessSite, int cause);
	int  OnStartServe(DWORD serveIp, int servePort, BOOL reuse, int listenNum, COSSema* sema, int* ret);
	void OnStopServe(COSSema* sema);
	void OnTrustSession(SOCKET sock, void* param, COSSema* sema = NULL);
};

RoseTcp_ServeMan::RoseTcp_ServeMan(IRoseTcpSessionMaker* sessionMaker, UINT32 maxSessions, UINT32 sessionKeepAlive)
	: m_sessionMaker(sessionMaker)
	, m_bindSock(INVALID_SOCKET)
	, m_thAcceptHandle(INVALID_OSTHREAD)
	, m_thAcceptId(INVALID_OSTHREADID)
	, m_exit(false)
	, m_maxSessionsPerThd(1024)
	, m_totalSessions(0)
	, m_needPollFdCount(0)
	, m_epoll(0)
	, m_sessionPool(NULL)
	, m_maxSessions(maxSessions)
	, m_sessionKeepAlive(sessionKeepAlive)
{
	SockInit();

	//忽略SIGPIPE信号
	signal(SIGPIPE, SIG_IGN);

	if (!UpdateSystemFdsLimited(m_maxSessions))
		printf("修改系统fd限制数失败，请检查是否root权限执行该程序(err=%d)\n", errno);

	m_epoll = epoll_create(64 * 1024);
	ASSERT(m_epoll > 0);
	OSCreateThread(&m_thAcceptHandle, &m_thAcceptId, THAcceptWorker, this, 0);
}

RoseTcp_ServeMan::~RoseTcp_ServeMan()
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

void RoseTcp_ServeMan::DeleteThis()
{
	delete this;
}

int RoseTcp_ServeMan::StartServe(const IPPADDR& serveIPP, BOOL reuse/* = true*/, int listenNum/* = 1024*/)
{
	if (m_thAcceptId == OSThreadSelf())
		return OnStartServe(serveIPP.ip_, serveIPP.port_, reuse, listenNum, NULL, NULL);
	else
	{
		COSSema sema;
		int retCode = ROSETCP_OK;

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
	return ROSETCP_OK;
}

void RoseTcp_ServeMan::StopServe(BOOL wait /*= true*/)
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

void RoseTcp_ServeMan::OnStopServe(COSSema* sema)
{
	//关闭会话
	SessionSiteSet::iterator it = m_sessionSiteSet.begin();
	for (; it != m_sessionSiteSet.end(); ++it)
	{
		SessionSite* sessSite = *it;
		CloseSession(sessSite->m_session, true);
	}

	//关闭accept操作
	if (m_epoll)
	{
		close(m_epoll);
		m_epoll = 0;
		m_needPollFdCount = 0;
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
		sessSite->m_session->OnClosed(ROSETCP_SELF_ACTIVE_CLOSED);
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

int RoseTcp_ServeMan::GetMaxSessionsPerThread() const
{
	return m_maxSessionsPerThd;
}

void RoseTcp_ServeMan::SetMaxSessionsPerIoThread(int maxSess/* = 1024*/)
{
	m_maxSessionsPerThd = max(maxSess, 1024);
}

int  RoseTcp_ServeMan::GetMaxSessionsPerIoThread() const
{
	return m_maxSessionsPerThd;
}

UINT32 RoseTcp_ServeMan::GetSessionsCount() const
{
	return (UINT32)m_sessionSiteSet.size();
}

void RoseTcp_ServeMan::SetSessionKeepAliveTime(UINT32 sec)
{
	m_sessionKeepAlive = sec;
}

UINT32 RoseTcp_ServeMan::GetSessionKeepAliveTime() const
{
	return m_sessionKeepAlive;
}

void RoseTcp_ServeMan::SetSessionSendBufSize(IRoseTcpSession* session, int nBytes)
{
	if (session == NULL || nBytes <= 0)
	{
		ASSERT(false);
		return;
	}
	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	sessSite->m_maxBufSize = nBytes;
}

int RoseTcp_ServeMan::GetSessionSendBufSize(IRoseTcpSession* session)
{
	if (session == NULL)
	{
		ASSERT(false);
		return 0;
	}

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	return sessSite->m_maxBufSize;
}

int RoseTcp_ServeMan::GetSessionBufNotSentSize(IRoseTcpSession* session)
{
	if (session == NULL)
	{
		ASSERT(false);
		return 0;
	}

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	return sessSite->m_notSentSize;
}

int RoseTcp_ServeMan::SendSessionData(IRoseTcpSession* session, const void* data, int len, BOOL canDiscard /*= true*/, BOOL needCopy/* = true*/, void* context/* = NULL*/)
{
	if (session == NULL || data == NULL || len <= 0)
		return ROSETCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	if (sessSite->m_ownerPollSite == NULL)
		return ROSETCP_ERROR_USE;

	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return ROSETCP_SESSION_CLOSING;

	if (canDiscard && sessSite->IsSendBufferFull())
		return ROSETCP_SENDBUF_IS_FULL;

	BOOL isPollThread = (OSThreadSelf() == sessSite->m_ownerPollSite->m_pollThread.m_id);
	RoseTcp_ServeMan::PollSite::PollSiteMsg msg;
	SendBuf sendBuf;
	if (needCopy)
	{
		void* p = RoseTcp_Alloc(len);
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
		sessSite->OnSendData(msg.sendData.buf, msg.sendData.needLen, msg.sendData.needFree, msg.sendData.context, msg.sendData.isEmergency);
	else
	{
		msg.sendData.sessSite = sessSite;
		msg.msg = RoseTcp_ServeMan::PollSite::mg_SendData;
		sessSite->m_ownerPollSite->PostMsg(msg);
	}
	return ROSETCP_OK;
}

int  RoseTcp_ServeMan::SendSessionDataVec(IRoseTcpSession* session, const SockBufVec* bufs, int bufsCount, BOOL canDiscard/* = true*/, void* context/* = NULL*/)
{
	if (session == NULL || bufs == NULL || bufsCount <= 0)
		return ROSETCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	if (sessSite->m_ownerPollSite == NULL)
		return ROSETCP_ERROR_USE;

	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return ROSETCP_SESSION_CLOSING;

	if (canDiscard && sessSite->IsSendBufferFull())
		return ROSETCP_SENDBUF_IS_FULL;

	int nBytes = 0;
	for (int i = 0; i < bufsCount; ++i)
	{
		if (bufs[i].buf != NULL && bufs[i].bufBytes > 0)
			nBytes += bufs[i].bufBytes;
	}

	if (nBytes <= 0)
		return ROSETCP_INVALID_PARAM;

	BYTE* p = (BYTE*)RoseTcp_Alloc(nBytes);
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
	RoseTcp_ServeMan::PollSite::PollSiteMsg msg;
	msg.sendData.context = context;
	msg.sendData.needFree = true;
	msg.sendData.buf = p;
	msg.sendData.isEmergency = false;
	msg.sendData.needLen = nBytes;
	if (isPollThread)
		sessSite->OnSendData(msg.sendData.buf, msg.sendData.needLen, msg.sendData.needFree, msg.sendData.context, msg.sendData.isEmergency);
	else
	{
		msg.sendData.sessSite = sessSite;
		msg.msg = RoseTcp_ServeMan::PollSite::mg_SendData;
		sessSite->m_ownerPollSite->PostMsg(msg);
	}
	return ROSETCP_OK;
}

int  RoseTcp_ServeMan::SendSessionEmergencyData(IRoseTcpSession* session, const void* data, int len, BOOL needCopy /*= true*/, void* context /*= NULL*/)
{
	if (session == NULL || data == NULL || len <= 0)
		return ROSETCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	if (sessSite->m_ownerPollSite == NULL)
		return ROSETCP_ERROR_USE;
	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return ROSETCP_SESSION_CLOSING;

	BOOL isPollThread = (OSThreadSelf() == sessSite->m_ownerPollSite->m_pollThread.m_id);
	RoseTcp_ServeMan::PollSite::PollSiteMsg msg;
	if (needCopy)
	{
		void* p = RoseTcp_Alloc(len);
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
	if (isPollThread)
	{
		sessSite->OnSendData(msg.sendData.buf, msg.sendData.needLen, msg.sendData.needFree, msg.sendData.context, true);
	}
	else
	{
		msg.sendData.sessSite = sessSite;
		msg.msg = RoseTcp_ServeMan::PollSite::mg_SendData;
		sessSite->m_ownerPollSite->PostEmergencyMsg(msg);
	}
	return ROSETCP_OK;
}

int  RoseTcp_ServeMan::SendSessionEmergencyDataVec(IRoseTcpSession* session, const SockBufVec* bufs, int bufsCount, void* context/* = NULL*/)
{
	if (session == NULL || bufs == NULL || bufsCount <= 0)
		return ROSETCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	if (sessSite->m_ownerPollSite == NULL)
		return ROSETCP_ERROR_USE;

	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return ROSETCP_SESSION_CLOSING;

	int nBytes = 0;
	for (int i = 0; i < bufsCount; ++i)
	{
		if (bufs[i].buf != NULL && bufs[i].bufBytes > 0)
			nBytes += bufs[i].bufBytes;
	}

	if (nBytes <= 0)
		return ROSETCP_INVALID_PARAM;

	BYTE* p = (BYTE*)RoseTcp_Alloc(nBytes);
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
	RoseTcp_ServeMan::PollSite::PollSiteMsg msg;
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
		msg.msg = RoseTcp_ServeMan::PollSite::mg_SendData;
		sessSite->m_ownerPollSite->PostEmergencyMsg(msg);
	}
	return ROSETCP_OK;
}

void RoseTcp_ServeMan::TrustSession(SOCKET sock, void* param /*= NULL*/, BOOL wait /*= false*/)
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

int RoseTcp_ServeMan::CloseSession(IRoseTcpSession* session, BOOL wait /*= false*/)
{
	if (session == NULL)
		return ROSETCP_INVALID_PARAM;

	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	ASSERT(sessSite->m_ownerPollSite);
	if (sessSite->m_ownerPollSite == NULL)
		return ROSETCP_ERROR_USE;

	if (sessSite->m_closeFlag || sessSite->m_prepareClose)
		return ROSETCP_SESSION_CLOSING;

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
	return ROSETCP_OK;
}

void RoseTcp_ServeMan::UpdateSessionKeepAliveTick(IRoseTcpSession* session)
{
	if (session == NULL)
	{
		ASSERT(false);
		return;
	}
	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	sessSite->m_sessionTick = GetTickCount();
}

void RoseTcp_ServeMan::EnableSessionKeepAliveCheck(IRoseTcpSession* session, BOOL enable)
{
	if (session == NULL)
	{
		ASSERT(false);
		return;
	}
	SessionSite* sessSite = (SessionSite*)((BYTE*)session - offset_of(SessionSite, m_sessionBuf));
	sessSite->m_enableKeepAliveCheck = enable;
}

int RoseTcp_ServeMan::THAcceptWorker(void* p)
{
	RoseTcp_ServeMan* pThis = (RoseTcp_ServeMan*)p;
	ASSERT(pThis != NULL);
	return pThis->OnAccept();
}

int RoseTcp_ServeMan::OnAccept()
{
	while (!m_exit)
	{
		BOOL idle = false;

		//处理消息
		idle = OnProcessServeManMsg();

		//处理连接上来
		if (m_needPollFdCount > 0)
		{
			int waitCount = min(MAX_FD_PER_WAIT, m_needPollFdCount);
			int ret = epoll_wait(m_epoll, m_waitEvent, waitCount, 0);
			if (ret > 0)
			{
				idle = false;
				for (int i = 0; i < ret; ++i)
				{
					const struct epoll_event& evt = m_waitEvent[i];
					uint32_t revents = evt.events;

					if (EPOLLIN & revents)	//可读，有连接上来了
					{
						IPPADDR peerIPP;
						SOCKET acceptSock = TCPAccept(m_bindSock, &peerIPP.ip_, &peerIPP.port_);
						if (SockValid(acceptSock))
						{
							int r = OnAfterAccepted(acceptSock, peerIPP);
							if (r != ROSETCP_OK)
								SockClose(acceptSock);
						}
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

int RoseTcp_ServeMan::OnAfterAccepted(SOCKET acceptSock, const IPPADDR& peerIPP, void* param/* = NULL*/, COSSema* sema/* = NULL*/)
{
	if (m_totalSessions >= m_maxSessions)	//超过了允许的最大会话数
	{
		printf("超过了服务最大会话数限制[%d]\n", m_maxSessions);
		return ROSETCP_INNER_ERROR;
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
			return ROSETCP_INVALID_SESSION_SIZE;
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
		if (p->GetPollSessionCount() < m_maxSessionsPerThd)
		{
			pollSite = p;
			break;
		}
	}

	if (NULL == pollSite)
	{
		pollSite = new RoseTcp_ServeMan::PollSite(this);
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

	return ROSETCP_OK;
}

BOOL RoseTcp_ServeMan::OnProcessServeManMsg()
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
		OnOnServeManMsg(*it);
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
		OnOnServeManMsg(*it);
#endif
	return idle;
}

void RoseTcp_ServeMan::OnOnServeManMsg(const ServeManMsg& msg)
{
	switch (msg.msg)
	{
	case mg_SessionBroken:
	{
		int r = OnSessionBroken(msg.brokenSession.sessSite, msg.brokenSession.brokenCause);
		if (r != ROSETCP_OK)
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

BOOL RoseTcp_ServeMan::OnRecyleGabageSessions()
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

void RoseTcp_ServeMan::OnTrustSession(SOCKET sock, void* param, COSSema* sema)
{
	IPPADDR peerIPP = SockGetPeer(sock);
	int r = OnAfterAccepted(sock, peerIPP, param, sema);
	if (r != 0 && sema != NULL)
		sema->Post();
}

int RoseTcp_ServeMan::OnSessionBroken(RoseTcp_ServeMan::SessionSite* sessSite, int cause)
{
	SessionSiteSet::iterator it = m_sessionSiteSet.find(sessSite);
	if (it == m_sessionSiteSet.end())
	{
		//ASSERT(false);
		return ROSETCP_OK;
	}

	if (sessSite->m_asyncReceivedCalling)	//还处于异步回调中不能回收
		return ROSETCP_SESSION_USING;

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
	return ROSETCP_OK;
}

int  RoseTcp_ServeMan::OnStartServe(DWORD serveIp, int servePort, BOOL reuse, int listenNum, COSSema* sema, int* ret)
{
	ASSERT(m_sessionMaker != NULL);
	int retCode = ROSETCP_OK;
	if (SockValid(m_bindSock))	//服务已经启动了
		retCode = ROSETCP_SERVE_STARTED;

	if (ret != NULL)
		*ret = retCode;

	if (retCode != ROSETCP_OK)
	{
		if (sema)
			sema->Post();
		return retCode;
	}

	if (m_sessionPool == NULL)
	{
		int sessionSize = m_sessionMaker->GetSessionObjSize();
		if (sessionSize <= 0)
			retCode = ROSETCP_INVALID_SESSION_SIZE;
		else
		{
			int unitSize = sizeof(SessionSite) + sessionSize - sizeof(INT_PTR);
			m_sessionPool = new SessionPool(unitSize, m_maxSessions, 64, 0);
			ASSERT(m_sessionPool != NULL);
		}
	}

	if (ret != NULL)
		*ret = retCode;

	if (retCode != ROSETCP_OK)
	{
		if (sema)
			sema->Post();
		return retCode;
	}

	SOCKET sock = TCPNewServerEx(serveIp, servePort, reuse, listenNum);
	if (!SockValid(sock))
	{
		if (EADDRINUSE == errno)
		{
			printf("Only one usage of each tcp serve, port: %d\n", servePort);
			retCode = ROSETCP_SYSTEM_ERROR;
		}
		else if (EADDRNOTAVAIL == errno)
		{
			printf("The requested address is not valid in its context, port: %d\n", servePort);
			retCode = ROSETCP_SYSTEM_ERROR;
		}
		else
		{
			printf("System call error for the creating, port: %d\n", servePort);
			retCode = ROSETCP_SYSTEM_ERROR;
		}
	}

	if (ret != NULL)
		*ret = retCode;

	if (retCode != ROSETCP_OK)
	{
		if (sema)
			sema->Post();
		return retCode;
	}

	SockSetNonBlock(sock, true);
	m_bindSock = sock;

	struct epoll_event epevt;
	epevt.events = EPOLLIN;
	epevt.events |= ERR_POLL_FLAG;
	memset(&epevt.data, 0, sizeof(epevt.data));
	epevt.data.ptr = NULL;
	int r = epoll_ctl(m_epoll, EPOLL_CTL_ADD, sock, &epevt);
	ASSERT(r == 0);
	++m_needPollFdCount;
	if (sema)
		sema->Post();
	return retCode;
}

void RoseTcp_ServeMan::OnPostServeManMsg(const ServeManMsg& msg)
{
	CXTAutoLock lock(m_msgLocker);
	m_msgQ.push_back(msg);
}

RoseTcp_ServeMan::PollSite::PollSite(RoseTcp_ServeMan* serveMan)
	: m_serveMan(serveMan)
	, m_exit(false)
	, m_pollfd(0)
	, m_lastKeepAliveCheckTick(0)
{

}

RoseTcp_ServeMan::PollSite::~PollSite()
{

}

BOOL RoseTcp_ServeMan::PollSite::Open()
{
	m_pollfd = epoll_create(m_serveMan->GetMaxSessionsPerThread());
	ASSERT(m_pollfd > 0);
	if (m_pollfd < 0)
		return false;
	return m_pollThread.Open(THPollWorker, this, 0);
}

void RoseTcp_ServeMan::PollSite::Close()
{
	m_exit = true;
	m_pollThread.Close();
	if (m_pollfd)
	{
		close(m_pollfd);
		m_pollfd = 0;
	}
}

int RoseTcp_ServeMan::PollSite::GetPollSessionCount()
{
	return (int)m_pollSessionSiteSet.size();
}

int RoseTcp_ServeMan::PollSite::THPollWorker(void* p)
{
	PollSite* pThis = (PollSite*)p;
	ASSERT(pThis != NULL);
	return pThis->OnPollWorker();
}

int RoseTcp_ServeMan::PollSite::OnPollWorker()
{
	while (!m_exit)
	{
		BOOL idle = false;

		//先处理消息
		idle = ProcessPollSiteMsg();

		int waitCount = min(MAX_FD_PER_WAIT, m_pollSessionSiteSet.size());
		if (waitCount <= 0)
		{
			if (idle)
			{
				Sleep(1);
				continue;
			}
		}

		int ret = epoll_wait(m_pollfd, m_waitEvent, waitCount, 0);
		if (ret < 0)	//有错 ???
		{
			//检查会话存活时间
			CheckSessionKeepAliveTime();
			continue;
		}

		if (ret == 0)
		{
			//检查会话存活时间
			CheckSessionKeepAliveTime();

			if (idle)
				Sleep(1);
			continue;
		}

		BOOL processIdle = true;
		for (int i = 0; i < ret; ++i)	//处理事件
		{
			BOOL ret = OnProcessEvent(m_waitEvent[i]);
			if (!ret)
				processIdle = false;
		}
		idle = processIdle;

		//检查会话存活时间
		CheckSessionKeepAliveTime();

		if (idle)
			Sleep(1);
	}
	return 0;
}

BOOL RoseTcp_ServeMan::PollSite::OnProcessEvent(const struct epoll_event& epollEvt)
{
	BOOL idle = false;
	uint32_t revents = epollEvt.events;
	RoseTcp_ServeMan::SessionSite* sessSite = (RoseTcp_ServeMan::SessionSite*)epollEvt.data.ptr;
	int brokenFlag = ROSETCP_OK;
	int readRet = ROSETCP_OK;
	int writeRet = ROSETCP_OK;
	if (revents & POLLIN)	//可读
		readRet = sessSite->OnReadSingal();

	if (revents & POLLOUT)	//可写
		writeRet = sessSite->OnWriteSingal(idle);

	if (revents & ERR_POLL_FLAG)	//有断线事件发生
		brokenFlag = ROSETCP_PEER_CLOSED;

	//将连接断掉
	if (brokenFlag != ROSETCP_OK)
		OnSessionBroken(sessSite, brokenFlag);
	else if (readRet != ROSETCP_OK)
		OnSessionBroken(sessSite, readRet);
	else if (writeRet != ROSETCP_OK)
		OnSessionBroken(sessSite, writeRet);

	return idle;
}

void RoseTcp_ServeMan::PollSite::CheckSessionKeepAliveTime()
{
	UINT32 keepAlive = m_serveMan->GetSessionKeepAliveTime() * 1000;	//毫秒
	if (keepAlive <= 0)
		return;

	DWORD nowTick = GetTickCount();
	DWORD interval = nowTick - m_lastKeepAliveCheckTick;
	if (interval < SESSION_KEEPALIVE_CHECK_TIMER)
		return;

	m_lastKeepAliveCheckTick = nowTick;
	std::set<RoseTcp_ServeMan::SessionSite*>::iterator it = m_pollSessionSiteSet.begin();
	for (; it != m_pollSessionSiteSet.end();)
	{
		RoseTcp_ServeMan::SessionSite* sessSite = *it;
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
			RoseTcp_ServeMan::ServeManMsg msg;
			msg.msg = RoseTcp_ServeMan::mg_SessionBroken;
			msg.brokenSession.sessSite = sessSite;
			msg.brokenSession.brokenCause = ROSETCP_KEEPALIVE_EXPIRED;
			m_serveMan->OnPostServeManMsg(msg);
		}
		else
			++it;
	}
}

void RoseTcp_ServeMan::PollSite::OnSessionBroken(RoseTcp_ServeMan::SessionSite* sessSite, int cause)
{
	if (m_pollSessionSiteSet.find(sessSite) == m_pollSessionSiteSet.end())
		return;

	//从epoll中移除, 这里不判断返回值,有可能失败
	epoll_ctl(m_pollfd, EPOLL_CTL_DEL, sessSite->m_sessionSock, NULL);

	sessSite->Broken();
	m_pollSessionSiteSet.erase(sessSite);

	//投递断线消息到ServeMan线程中
	sessSite->m_closeFlag = true;
	RoseTcp_ServeMan::ServeManMsg msg;
	msg.msg = RoseTcp_ServeMan::mg_SessionBroken;
	msg.brokenSession.sessSite = sessSite;
	msg.brokenSession.brokenCause = cause;
	m_serveMan->OnPostServeManMsg(msg);
}

BOOL RoseTcp_ServeMan::PollSite::ProcessPollSiteMsg()
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

void RoseTcp_ServeMan::PollSite::PostMsg(const PollSiteMsg& msg)
{
	CXTAutoLock lock(m_msgQLocker);
	m_msgQ.push_back(msg);
}

void RoseTcp_ServeMan::PollSite::PostEmergencyMsg(const PollSiteMsg& msg)
{
	CXTAutoLock lock(m_msgQLocker);
	m_msgQ.push_front(msg);
}

void RoseTcp_ServeMan::PollSite::OnMsg(const PollSiteMsg& msg)
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

void RoseTcp_ServeMan::PollSite::OnAddSession(UINT_PTR completeKey, COSSema* sema)
{
	RoseTcp_ServeMan::SessionSite* sessSite = (RoseTcp_ServeMan::SessionSite*)completeKey;
	ASSERT(m_pollSessionSiteSet.find(sessSite) == m_pollSessionSiteSet.end());

	SOCKET sessionSock = sessSite->m_sessionSock;
	ASSERT(SockValid(sessionSock));
	sessSite->m_sessionTick = GetTickCount();
	m_pollSessionSiteSet.insert(sessSite);

	struct epoll_event epevt;
	epevt.events = EPOLLIN | EPOLLOUT;
	epevt.events |= ERR_POLL_FLAG;
	memset(&epevt.data, 0, sizeof(epevt.data));
	epevt.data.ptr = sessSite;
	int r = epoll_ctl(m_pollfd, EPOLL_CTL_ADD, sessionSock, &epevt);
	ASSERT(r == 0);

	sessSite->ConnectedSend();
	r = sessSite->RequestReceive();
	if (sema != NULL)
		sema->Post();

	if (r != ROSETCP_OK)
		OnSessionBroken(sessSite, r);
}

void RoseTcp_ServeMan::PollSite::OnDelSession(RoseTcp_ServeMan::SessionSite* sessSite, COSSema* sema)
{
	OnSessionBroken(sessSite, ROSETCP_SELF_ACTIVE_CLOSED);
	if (sema != NULL)
		sema->Post();
}

void RoseTcp_ServeMan::PollSite::OnSendData(RoseTcp_ServeMan::SessionSite* sessSite, const void* data, int len, BOOL needFree, void* context, BOOL isEmergency/* = false*/)
{
	ASSERT(OSThreadSelf() == m_pollThread.m_id);
	if (m_pollSessionSiteSet.find(sessSite) != m_pollSessionSiteSet.end())
		sessSite->OnSendData(data, len, needFree, context, isEmergency);
	else
	{
		if (needFree)
			RoseTcp_Free((void*)data);
		else
			ASSERT(false);
	}
}

void RoseTcp_ServeMan::PollSite::OnAsyncReceivedCalled(int onReceivedRet, RoseTcp_ServeMan::SessionSite* sessSite)
{
	ASSERT(sessSite->m_asyncReceivedCalling);
	ASSERT(sessSite->m_recvBuf.m_needLen >= sessSite->m_recvBuf.m_receivedLen);
	if (sessSite->m_prepareClose || sessSite->m_closeFlag)
	{
		sessSite->m_asyncReceivedCalling = false;
		return;
	}

	sessSite->m_recvBuf.Close();
	sessSite->m_asyncReceivedCalling = false;

	struct epoll_event epevt;
	epevt.events = EPOLLIN | EPOLLOUT;
	epevt.events |= ERR_POLL_FLAG;
	memset(&epevt.data, 0, sizeof(epevt.data));
	epevt.data.ptr = sessSite;
	int r = epoll_ctl(m_pollfd, EPOLL_CTL_ADD, sessSite->m_sessionSock, &epevt);
	ASSERT(r == 0);

	if (onReceivedRet != ROSETCP_OK)	///将连接断掉
		OnSessionBroken(sessSite, onReceivedRet);
	else
	{
		r = sessSite->RequestReceive();
		if (r != ROSETCP_OK)	///将连接断掉
			OnSessionBroken(sessSite, r);
	}
}

RoseTcp_ServeMan::SessionSite::SessionSite(/*RoseTcp_ServeMan* serveMan*/)
	: m_session((RoseTcpSessionTmpl*)m_sessionBuf)
	// , m_serveMan(serveMan)
{
	Init();
}

RoseTcp_ServeMan::SessionSite::~SessionSite()
{
	Close();
	Init();
	m_session->~RoseTcpSessionTmpl();
}

void RoseTcp_ServeMan::SessionSite::Init()
{
	m_closeFlag = false;
	m_prepareClose = false;
	m_ownerPollSite = NULL;
	m_sessionSock = INVALID_SOCKET;
	m_maxBufSize = 0;
	m_notSentSize = 0;
	m_asyncReceivedCalling = false;
	m_sessionTick = 0;
	m_enableKeepAliveCheck = true;
	ASSERT((void*)m_session == (void*)m_sessionBuf);
}

void RoseTcp_ServeMan::SessionSite::Close()
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

void RoseTcp_ServeMan::SessionSite::Broken()
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

BOOL RoseTcp_ServeMan::SessionSite::IsSendBufferFull()
{
	if (m_maxBufSize <= 0)
		return false;
	return  m_notSentSize >= m_maxBufSize ? true : false;
}

void RoseTcp_ServeMan::SessionSite::ConnectedSend()
{
	ASSERT(m_session != NULL);
	if (m_session != NULL)
		m_session->OnConnectedSend();
}

int RoseTcp_ServeMan::SessionSite::RequestReceive()
{
	ASSERT(m_recvBuf.IsNull());
	ASSERT(!m_asyncReceivedCalling);
	void* buf = NULL;
	int len = 0;
	BOOL asynNotify = false;
	IRoseTcpSession::IoReadType rt = IRoseTcpSession::io_maxRead;
	int r = m_session->RequestReceive(buf, len, rt, asynNotify);
	if (r != ROSETCP_OK)
		return r;
	if (len <= 0)
		return ROSETCP_INVALID_READ_SIZE;

	if (buf == NULL)
	{
		buf = RoseTcp_Alloc(len);
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
	return ROSETCP_OK;
}

int RoseTcp_ServeMan::SessionSite::OnReadSingal()
{
	ASSERT(!m_recvBuf.IsNull());
	int ret = 0;
	char* p = (char*)m_recvBuf.m_buf + m_recvBuf.m_receivedLen;
	int len = m_recvBuf.m_needLen - m_recvBuf.m_receivedLen;
	int n = recv(m_sessionSock, p, len, 0);
	if (n == 0)
		ret = ROSETCP_SESSION_BROKEN;
	else if (n < 0)
	{
		if (errno == EAGAIN || errno == EINTR)
			ret = ROSETCP_OK;
		else
			ret = ROSETCP_SESSION_BROKEN;
	}
	if (ret != ROSETCP_OK)
		return ret;

	if (n > 0)
		m_recvBuf.m_receivedLen += n;
	BOOL canCallOnReceived = false;
	if (IRoseTcpSession::io_maxRead == m_recvBuf.m_iort || m_recvBuf.m_receivedLen == m_recvBuf.m_needLen)
		canCallOnReceived = true;
	if (!canCallOnReceived)
		return ret;

	BOOL continueRecv = true;
	if (m_recvBuf.m_notifyByTaskPool)
	{
		epoll_ctl(m_ownerPollSite->m_pollfd, EPOLL_CTL_DEL, m_sessionSock, NULL);
		m_asyncReceivedCalling = true;
		RoseCore_FetchTaskPool()->Push(this);
	}
	else
	{
		ret = m_session->OnReceived(m_recvBuf.m_buf, m_recvBuf.m_receivedLen, false, continueRecv);
		m_recvBuf.Close();
		if (ret != ROSETCP_OK)
			return ret;
		ret = RequestReceive();
	}
	return ret;
}

int RoseTcp_ServeMan::SessionSite::OnWriteSingal(BOOL& idle)
{
	if (m_sendBuf.IsNull())
	{
		if (m_sendBufQ.empty())
		{
			idle = true;
			return ROSETCP_OK;
		}

		m_sendBuf = *m_sendBufQ.begin();
		m_sendBufQ.erase(m_sendBufQ.begin());
	}

	idle = false;
	int ret = 0;
	char* p = (char*)m_sendBuf.m_buf + m_sendBuf.m_sentLen;
	int len = m_sendBuf.m_needLen - m_sendBuf.m_sentLen;
	int n = send(m_sessionSock, p, len, 0);
	if (n < 0)
	{
		if (errno == EAGAIN || errno == EINTR)
			ret = ROSETCP_OK;
		else
			ret = ROSETCP_SESSION_BROKEN;
	}

	if (ret != ROSETCP_OK)
	{
		m_sendBuf.Close();
		return ret;
	}

	if (n > 0)
		m_sendBuf.m_sentLen += n;
	if (m_sendBuf.m_sentLen == m_sendBuf.m_needLen)
	{
		m_notSentSize -= m_sendBuf.m_needLen;
		ASSERT(m_notSentSize >= 0);
		m_session->OnSent(m_sendBuf.m_buf, m_sendBuf.m_sentLen, m_sendBuf.m_needFree, m_sendBuf.m_context, true);
		m_sendBuf.Close();
	}
	return ret;
}

void RoseTcp_ServeMan::SessionSite::OnSendData(const void* data, int len, BOOL needFree, void* context, BOOL isEmergency/* = false*/)
{
	if (data == NULL || len <= 0)
		return;
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

void RoseTcp_ServeMan::SessionSite::OnPoolTask()
{
	ASSERT(m_asyncReceivedCalling);
	BOOL continueRecv = true;
	int r = m_session->OnReceived(m_recvBuf.m_buf, m_recvBuf.m_receivedLen, true, continueRecv);
	if (m_prepareClose || m_closeFlag)
	{
		m_asyncReceivedCalling = false;
		return;
	}

	RoseTcp_ServeMan::PollSite::PollSiteMsg msg;
	msg.msg = RoseTcp_ServeMan::PollSite::mg_AsyncReceivedCalled;
	msg.asyncReceivedCalled.onReceivedRet = r;
	msg.asyncReceivedCalled.sessSite = this;
	m_ownerPollSite->PostMsg(msg);
}

ROSETCP_API int RoseTcp_MakeServeMan(IRoseTcpServeMan*& serveMan,
	IRoseTcpSessionMaker* sessionMaker,
	UINT32 maxSessions/* = 64*1024*/,
	UINT32 sessionKeepAlive/* = 0*/)
{
	RoseTcp_ServeMan* man = new RoseTcp_ServeMan(sessionMaker, maxSessions, sessionKeepAlive);
	if (man == NULL)
		return ROSETCP_ALLOC_MEM_FAILED;
	serveMan = man;
	return ROSETCP_OK;
}

///分配内存
ROSETCP_API void* RoseTcp_Alloc(INT_PTR si)
{
#if USE_MEM_POOL
	return GetTCPSmallMemPoolObj__().m_poolObj->Alloc(si);
#else
	return ::malloc(si);
#endif
}

///释放内存
ROSETCP_API void RoseTcp_Free(void* p)
{
	if (p == NULL)
		return;
#if USE_MEM_POOL
	GetTCPSmallMemPoolObj__().m_poolObj->Free(p);
#else
	::free(p);
#endif
}

#endif//__shitandfuck__