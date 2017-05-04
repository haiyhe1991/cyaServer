#ifndef __CYA_TCP_ERR_H__
#define __CYA_TCP_ERR_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif


#define		CYATCP_OK						0		//操作成功
#define		CYATCP_ERROR					-1		//未知错误
#define		CYATCP_INVALID_PARAM			-10001	//参数不合法
#define		CYATCP_ERROR_USE				-10002	//用法错误
#define		CYATCP_SESSION_CLOSING			-10003	//会话处于关闭中
#define		CYATCP_INNER_ERROR				-10004	//内部错误
#define		CYATCP_INVALID_SESSION_SIZE		-10005	//错误的session大小
#define		CYATCP_SYSTEM_ERROR				-10006	//系统错误
#define		CYATCP_SERVE_STARTED			-10007	//服务处于启动中
#define		CYATCP_SESSION_USING			-10008	//会话处于使用中
#define		CYATCP_SESSION_BROKEN			-10009	//会话端线
#define		CYATCP_INVALID_READ_SIZE		-10010	//要读取的数据长度非法
#define		CYATCP_ALLOC_MEM_FAILED			-10011	//分配内存失败
#define		CYATCP_SENDBUF_IS_FULL			-10012	//会话发送缓冲区满
#define		CYATCP_PEER_CLOSED				-10013	//对端关闭连接
#define		CYATCP_SELF_ACTIVE_CLOSED		-10014	//自己主动关闭连接
#define		CYATCP_KEEPALIVE_EXPIRED		-10015	//存活时间过期,关闭连接

#endif