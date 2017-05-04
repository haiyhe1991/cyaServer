#ifndef __SERVICE_LINKER_H__
#define __SERVICE_LINKER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

//用户登陆linekr
struct SLoginLinkerReq	//请求
{
	UINT32 userId;		/// 账户ID
	char token[33];		///	登陆验证Token

	void hton()
	{
		userId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
	}
};

//用户退出linker
// struct SLogoutLinkerReq		//请求
// {
// 	UINT32 roleId;		///	角色ID
// 
// 	void hton()
// 	{
// 		roleId = htonl(roleId);
// 	}
// 
// 	void ntoh()
// 	{
// 		roleId = ntohl(roleId);
// 	}
// };

struct SConnectLinkerRes
{
	UINT32 heartSec;	/// 心跳时间间隔 单位秒
	char key[33];		/// 令牌密钥

	void hton()
	{
		heartSec = htonl(heartSec);
	}

	void ntoh()
	{
		heartSec = ntohl(heartSec);
	}
};


#endif	//_SGSLinker_h__