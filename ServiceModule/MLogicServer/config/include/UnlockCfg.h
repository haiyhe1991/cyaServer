#ifndef __UNLOCK_CFG_H__
#define __UNLOCK_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��������
*/

#include <map>
#include "CfgCom.h"

class UnlockCfg
{
public:
	UnlockCfg();
	~UnlockCfg();

#pragma pack(push, 1)
	struct SUnlockItem
	{
		BYTE isLock;	//�Ƿ�����
		UINT32 gold;	//�����������
		UINT32 rpcoin;	//������������
		BYTE   lvLimit;	//�ȼ�����
	};
	struct SUnlockObj
	{
		std::map<UINT16/*����id*/, SUnlockItem> unlockItemMap;
	};
#pragma pack(pop)

public:
	//���������ļ�
	BOOL Load(const char* filename);
	//��ȡ��������
	int GetUnlockItem(BYTE type, UINT16 unlockId, SUnlockItem*& pUnlockItem);
	//��ȡδ����id
	int GetUnlockedItems(BYTE type, std::vector<UINT16>& itemsVec);

private:
	std::map<BYTE/*��������*/, SUnlockObj> m_unlockCfgMap;
};


#endif