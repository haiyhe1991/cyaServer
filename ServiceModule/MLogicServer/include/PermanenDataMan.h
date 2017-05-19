#ifndef __PERMANEN_DATA_MAN_H__
#define __PERMANEN_DATA_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��ɫ��������(������Ϣ,ħ����)
*/

#include <map>
#include <vector>
#include "ServiceMLogic.h"
#include "cyaCoretimer.h"

enum EPermanenDataType
{
	PERMANENT_HELP = 1,			//����������Ϣ
	PERMANENT_MAGIC = 2,		//ħ������Ϣ
};

#define PERMANENT_DATA_ID_START		PERMANENT_HELP
#define PERMANENT_DATA_ID_END		PERMANENT_MAGIC

class PermanenDataMan
{
	PermanenDataMan(const PermanenDataMan&);
	PermanenDataMan& operator = (const PermanenDataMan&);

	struct SUpdatePermanenData	//��Ҫ���µĽ�ɫ
	{
		UINT32 userId;	//�û�id
		UINT32 roleId;	//��ɫid
		UINT16 type;	//��Ҫ����״̬

		SUpdatePermanenData()
		{
			userId = 0;
			roleId = 0;
			type = 0;
		}

		SUpdatePermanenData(UINT32 user, UINT32 role, UINT16 nType)
		{
			userId = user;
			roleId = role;
			type = nType;
		}
	};

public:
	PermanenDataMan();
	~PermanenDataMan();

public:
	//��ѯ�������������
	int QueryRoleFinishedHelps(UINT32 roleId, SQueryFinishedHelpRes* pHelpRes);
	//��ѯ��ɫӵ��ħ����
	int QueryRoleMagics(UINT32 roleId, SQueryRoleMagicsRes* pMagicRes);
	//��ʼ���½�ɫ��������
	int InitNewRolePermanenData(UINT32 roleId, const std::vector<UINT16>& magicVec);
	//������Ϣ
	void InputUpdatePermanenDataInfo(UINT32 userId, UINT32 roleId, UINT16 type);
	//�û��˳�ʱͬ��
	void ExitSyncUserPermanenData(UINT32 userId, const std::vector<UINT32>& rolesVec);

private:
	static void RefreshPermanenDataTimer(void* param, TTimerID id);
	void OnRefreshPermanenData();
	void OnRefreshPermanenData(const SUpdatePermanenData& info);

private:
	void OnUpdateRoleHelps(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleMagics(UINT32 userId, UINT32 roleId);

private:
	//��ɫ������Ϣ
	CXTLocker m_locker;
	CyaCoreTimer m_refreshDBTimer;
	std::map<std::string/*key*/, SUpdatePermanenData> m_updateInfoMap;
};

void InitPermanenDataMan();
PermanenDataMan* GetPermanenDataMan();
void DestroyPermanenDataMan();

#endif	//_PermanenDataMan_h__