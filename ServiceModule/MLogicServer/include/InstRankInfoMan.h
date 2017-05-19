#ifndef __INST_RANK_INFO_MAN_H__
#define __INST_RANK_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�������������Ϣ
*/

#include <map>
#include <list>
#include "ServiceMLogic.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"
#include "RoleInfos.h"

class InstRankInfoMan
{
private:
	InstRankInfoMan(const InstRankInfoMan&);
	InstRankInfoMan& operator = (const InstRankInfoMan&);

	struct SInstRankTable	//����������б�
	{
		UINT32 roleId;		//��ɫid
		UINT32 userId;		//�û�id
		char   roleNick[33];//��ɫ�ǳ�
		UINT32 score;		//����
		char   tts[20];		//���ʱ��1970-01-01 00:00:00
		UINT32 uniqueId;	//�����ݿ��е�Ψһid
		BYTE   up;			//�Ƿ���������е�ĳ����¼

		SInstRankTable()
		{
			memset(this, 0, sizeof(SInstRankTable));
		}
	};

	class InstRankSite
	{
	public:
		InstRankSite(UINT16 instId);
		~InstRankSite();

	public:
		void UpdateInstRank(SInstRankTable& rank, std::list<SInstOccupyEmail>& emailList);
		void InsertInstRank(SInstRankTable& rank);
		BYTE QueryInstRankInfo(SInstRankInfo* pRankInfo, BYTE rankInfoNum);
		void SyncDBInstRank();
		int  GetOccupyInstIdByRoleId(UINT32 roleId, UINT16& instId);
		int  GetInstOccupyInfo(UINT32& userId, UINT32& roleId);
		void DelRoleInstRank(UINT32 roleId);

	private:
		UINT16 m_instId;
		BYTE m_instGameMode;
		CXTLocker m_rankListLocker;
		std::list<SInstRankTable> m_rankList;
	};

public:
	InstRankInfoMan();
	~InstRankInfoMan();

	//����ռ���ɫ
	struct SInstOccupyRole
	{
		UINT16 instId;
		UINT32 roleId;
		UINT32 userId;
	};

	//���븱��������Ϣ
	void LoadInstRankInfo();

	//���¸���������б�
	void UpdateInstRankInfo(UINT16 instId, UINT32 userId, const SInstRankInfo& rank, std::list<SInstOccupyEmail>& emailList);

	//��ѯ������Ϣ
	BYTE QueryInstRankInfo(UINT16 instId, SInstRankInfo* pRankInfo, BYTE rankInfoNum);

	//��ȡ��ɫռ�츱��
	int GetRoleOccupyInst(UINT32 userId, UINT32 roleId, SGetRoleOccupyInstRes* pOccupyInstRes, int maxNum);

	//��ȡ����ռ����Ϣ
	void GetInstOccupyInfo(std::list<SInstOccupyRole>& occupyList);

	//ɾ����ɫ��������
	void DelRoleInstRank(UINT32 roleId);

private:
	void InsertDBInstRankTable(UINT16 instId, SInstRankTable& instRank);
	static void SyncInstRankTimer(void* param, TTimerID id);
	void OnSyncInstRank();

private:
	CXTLocker m_locker;
	std::map<UINT16/*����id*/, InstRankSite*> m_instRankMap;
	CyaCoreTimer m_refreshDBTimer;		//ˢ���ݿ�timer
};

//��ʼ���������������Ϣ
void InitInstRankInfoMan();
InstRankInfoMan* GetInstRankInfoMan();
void DestroyInstRankInfoMan();

#endif	//_InstRankInfoMan_h__