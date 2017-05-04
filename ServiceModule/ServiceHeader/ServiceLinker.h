#ifndef __SERVICE_LINKER_H__
#define __SERVICE_LINKER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

//�û���½linekr
struct SLoginLinkerReq	//����
{
	UINT32 userId;		/// �˻�ID
	char token[33];		///	��½��֤Token

	void hton()
	{
		userId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
	}
};

//�û��˳�linker
// struct SLogoutLinkerReq		//����
// {
// 	UINT32 roleId;		///	��ɫID
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
	UINT32 heartSec;	/// ����ʱ���� ��λ��
	char key[33];		/// ������Կ

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