#ifndef __GM_GLOBAL_OBJ_MAN_H__
#define __GM_GLOBAL_OBJ_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

//会话构造器;
#include "cyaTcp.h"
//会话;
#include "GMSdkSession.h"

//class GMSdkSesssionMaker;
class GMGlobalObjMan
{
public:
	GMGlobalObjMan();
	~GMGlobalObjMan();

	ICyaTcpServeMan* GetServeMan()
	{
		return m_serveMan;
	}


private:
	ICyaTcpServeMan* m_serveMan;
	GMSdkSesssionMaker m_gmsdkSessionMaker;
};

//初始化库;
void InitGMGlobalObjMan();
//返回会话的对象指针;
GMGlobalObjMan* GetGMGlobalObjMan();
//删除会话对象;
void DestroyGMGlobalObjMan();

#endif