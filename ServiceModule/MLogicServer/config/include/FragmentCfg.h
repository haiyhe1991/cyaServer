#ifndef __FRAGMENT_CFG_H__
#define __FRAGMENT_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��Ƭ����
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class FragmentCfg
{
public:
	FragmentCfg();
	~FragmentCfg();

#pragma pack(push, 1)
	struct SFragmentCfgAttr
	{
		BYTE quality;	//Ʒ��
		UINT16 maxStack;	//����
		SFragmentCfgAttr()
		{
			quality = 0;
			maxStack = 0;
		}
	};

	struct SGiveFragmentInfo
	{
		UINT16 id;
		UINT16 num;

		SGiveFragmentInfo()
		{
			id = 0;
			num = 0;
		}
	};

#pragma pack(pop)

public:
	//��������
	BOOL Load(const char* filename);
	//����������Ƭ
	BOOL LoadGiveFragment(const char* filename);
	//��ȡ�����û����ۼ۸�(����)
	int GetFragmentUserSellPrice(UINT16 fragmentId, UINT64& price);
	//��ȡ��Ƭ����
	int GetFragmentAttr(UINT16 fragmentId, SFragmentCfgAttr& fragmentAttr);
	//��ȡ������Ƭ
	int GetGiveFragment(std::vector<SGiveFragmentInfo>& giveVec);

private:
	std::map<UINT16/*��Ƭid*/, SFragmentCfgAttr> m_fragmentMap;
	std::vector<SGiveFragmentInfo> m_giveFragmentVec;
};


#endif