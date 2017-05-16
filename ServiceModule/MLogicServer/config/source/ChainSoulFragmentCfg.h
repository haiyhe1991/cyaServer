#ifndef __CHAIN_SOUL_FRAGMENT_CFG_H__
#define __CHAIN_SOUL_FRAGMENT_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
������Ƭ����
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class ChainSoulFragmentCfg
{
public:
	struct ChainSoulFragmentAttr
	{
		BYTE type;			//����
		BYTE quality;		//Ʒ��
		UINT32 maxStack;	//�������
	};

public:
	ChainSoulFragmentCfg();
	~ChainSoulFragmentCfg();

public:
	//��������
	BOOL Load(const char* filename);
	//��ȡ�����û����ۼ۸�(����)
	int GetFragmentUserSellPrice(UINT32 fragmentId, UINT64& price);
	//��ȡ������Ƭ����
	bool GetChainSoulFragmentAttr(UINT32 fragmentId, ChainSoulFragmentAttr &out_attr);
	//ͨ�����ͻ�ȡ������ƬID
	bool GetFragmentIdByType(BYTE type, UINT32 &out_id);

private:
	std::map<BYTE, UINT32> m_chainSoulFragmentType;
	std::map<UINT32, ChainSoulFragmentAttr> m_chainSoulFragment;
};

#endif