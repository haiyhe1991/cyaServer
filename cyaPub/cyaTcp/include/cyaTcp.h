#ifndef __CYA_TCP_H__
#define __CYA_TCP_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaSock.h"
#include "cyaTcpExports.h"
#include "cyaTcpErr.h"

///会话构造器
struct ICyaTcpSessionMaker
{
	virtual ~ICyaTcpSessionMaker(){}

	///获取session大小,总是这样实现{return sizeof(XXXSession);}
	virtual int GetSessionObjSize() = 0;

	///构造session对象,总是这样实现{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session) = 0;
};

///会话
struct ICyaTcpSession
{
	enum IoReadType
	{
		io_mustRead,
		io_maxRead
	};
	virtual ~ICyaTcpSession(){}

	///@sock[IN]	: 连接上来的SOCKET
	///@peerIPP[IN] : 对端ip地址信息
	///@sessionCount[IN] : 会话计数
	///@param[IN] : 当调用托管会话接口时，param为TrustSession传入的参数
	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL) = 0;

	///一连接上来需要立即发送数据(可在此调用ICyaTcpServeMan中的发送数据接口)
	virtual void OnConnectedSend() = 0;

	///连接断开
	///@cause[IN] : 连接断开原因
	virtual void OnClosed(int cause) = 0;

	///@buf[OUT]    : 数据被读入的缓冲区(若传空则由库自动分配管理)
	///@bufLen[OUT] : 需要读取数据的长度(>0)
	///@rt[OUT]		: 读取类型(io_maxRead->只要读到数据就触发OnReceived, io_mustRead->必须读到bufLen才触发OnReceived)
	///@asynNotify[OUT] : true/false->同步/异步调用OnReceived
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify) = 0;

	///@buf[IN] : 读取数据缓冲区
	///@bufLen[IN] : 数据长度
	///@callbackByTask[IN] : 调用方式(同RequestReceive中asynNotify传出值)
	///@continueRecv[IN] : true/false->是否继续读取
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv) = 0;

	///数据发送通知
	///@buf[IN] : 发送数据的buffer
	///@bufLen[IN] : 发送数据长度
	///@isCopy[IN] : 数据是否拷贝过
	///@context[IN] : 发送上下文参数
	///@isSendSuccess[IN] : 数据是否发送成功
	virtual void OnSent(const void* buf, int bufLen, BOOL isCopy, void* context, BOOL isSendSuccess) = 0;
};

//会话管理
struct ICyaTcpServeMan
{
	virtual ~ICyaTcpServeMan(){}

	///启动服务
	///@serveIPP[IN] : 服务ip地址信息
	///@reuse[IN] :	   是否设置监听SOCKET SO_REUSEADDR属性
	///@listenNum[IN] : listen数量
	virtual int  StartServe(const IPPADDR& serveIPP, BOOL reuse = true, int listenNum = 1024) = 0;

	///停止服务
	virtual void StopServe(BOOL wait = true) = 0;

	///设置服务中session的存活时间
	///@data[sec] : 存活时间(单位:秒, 0-永久)
	virtual void SetSessionKeepAliveTime(UINT32 sec) = 0;

	///获取服务中session的存活时间(单位:秒, 0-永久)
	virtual UINT32 GetSessionKeepAliveTime() const = 0;

	///设置单个线程管理的会话数
	///@maxSess[IN] : 单个线程管理的会话数
	virtual void SetMaxSessionsPerIoThread(int maxSess = 1024) = 0;

	///获取当前单个线程管理的会话数量
	virtual int  GetMaxSessionsPerIoThread() const = 0;

	///获取当前会话数量
	virtual UINT32 GetSessionsCount() const = 0;

	///发送数据
	///@session[IN] : 会话session
	///@data[IN]    : 要发送数据buffer地址
	///@len[IN]     : 发送数据长度
	///@canDiscard[IN] : 该数据包能否丢弃
	///@needCopy[IN] : 是否需要拷贝数据
	///@context[IN] : 发送上下文参数(由OnSent传回)
	virtual int  SendSessionData(ICyaTcpSession* session, const void* data, int len, BOOL canDiscard = true, BOOL needCopy = true, void* context = NULL) = 0;

	///发送数据(可发送多个buffer,数据要经过内部一次拷贝)
	///@session[IN] : 会话session
	///@bufs[IN]    : SockBufVec数组
	///@bufsCount[IN]     : SockBufVec数组元素个数
	///@canDiscard[IN] : 该数据包能否丢弃
	///@context[IN] : 发送上下文参数(由OnSent传回)
	virtual int  SendSessionDataVec(ICyaTcpSession* session, const SockBufVec* bufs, int bufsCount, BOOL canDiscard = true, void* context = NULL) = 0;

	///发送紧急数据(提高发送优先级,不可被丢弃,注意:使用该接口可能出现后发送的数据先到的情况,请谨慎使用)
	///@session[IN] : 会话session
	///@data[IN]    : 要发送数据buffer地址
	///@len[IN]     : 发送数据长度
	///@needCopy[IN] : 是否需要拷贝数据
	///@context[IN] : 发送上下文参数(由OnSent传回)
	virtual int  SendSessionEmergencyData(ICyaTcpSession* session, const void* data, int len, BOOL needCopy = true, void* context = NULL) = 0;

	///发送紧急数据(可发送多个buffer,数据要经过内部一次拷贝,提高发送优先级,不可被丢弃,注意:使用该接口可能出现后发送的数据先到的情况,请谨慎使用)
	///@session[IN] : 会话session
	///@bufs[IN]    : SockBufVec数组
	///@bufsCount[IN]     : SockBufVec数组元素个数
	///@context[IN] : 发送上下文参数(由OnSent传回)
	virtual int  SendSessionEmergencyDataVec(ICyaTcpSession* session, const SockBufVec* bufs, int bufsCount, void* context = NULL) = 0;

	///设置会话发送缓冲大小
	///@session[IN] : 会话session
	///@nBytes[IN] : 会话发送缓冲区缓存字节数(0-无限制)
	virtual void SetSessionSendBufSize(ICyaTcpSession* session, int nBytes) = 0;

	///获取会话发送缓冲大小(字节)
	///@session[IN] : 会话session
	virtual int GetSessionSendBufSize(ICyaTcpSession* session) = 0;

	///获取会话当前发送缓冲区未发送数据大小(字节)
	///@session[IN] : 会话session
	virtual int GetSessionBufNotSentSize(ICyaTcpSession* session) = 0;

	///托管会话
	///@sock[IN] : 要托管的SOCKET
	///@param[IN] : OnConnected传出的param
	///@wait[IN] : 是否等待OnConnected调用
	virtual void TrustSession(SOCKET sock, void* param = NULL, BOOL wait = false) = 0;

	///关闭会话
	///@session[IN] : 会话session
	///@wait[IN] : 是否等待socket关闭
	virtual int  CloseSession(ICyaTcpSession* session, BOOL wait = false) = 0;

	//更新会话存活时间
	///@session[IN] : 会话session
	virtual void UpdateSessionKeepAliveTick(ICyaTcpSession* session) = 0;

	//是否对该会话存活时间检测
	///@session[IN] : 会话session
	virtual void EnableSessionKeepAliveCheck(ICyaTcpSession* session, BOOL enable) = 0;

	///删除ICyaTcpServeMan对象
	virtual void DeleteThis() = 0;
};

//使用者必须从CyaTcpSessionTmpl继承
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


///构造IRoseTcpServeMan对象
///@serveMan[OUT] : IRoseTcpServeMan对象地址
///@sessionMaker[IN] : 会话构造器对象
///@maxSessions[IN] : 服务最大支持的会话数
///@sessionKeepAlive[IN] : 服务中会话的存活时间(单位:秒,0-永久)
CYATCP_API int CyaTcp_MakeServeMan(ICyaTcpServeMan*& serveMan,
									ICyaTcpSessionMaker* sessionMaker,
									UINT32 maxSessions = 64 * 1024,
									UINT32 sessionKeepAlive = 0);

///分配内存
CYATCP_API void* CyaTcp_Alloc(INT_PTR si);

///释放内存
CYATCP_API void CyaTcp_Free(void* p);

//构造释放对象
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