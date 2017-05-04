#ifndef __GM_GLOBAL_OBJ_MAN_H__
#define __GM_GLOBAL_OBJ_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

//�Ự������;
#include "cyaTcp.h"
//�Ự;
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

//��ʼ����;
void InitGMGlobalObjMan();
//���ػỰ�Ķ���ָ��;
GMGlobalObjMan* GetGMGlobalObjMan();
//ɾ���Ự����;
void DestroyGMGlobalObjMan();

#endif