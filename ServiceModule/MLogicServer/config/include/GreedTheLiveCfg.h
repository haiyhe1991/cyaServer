#ifndef __GREED_THE_LIVE_CFG_H__
#define __GREED_THE_LIVE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class GreedTheLiveCfg
{
public:
	// �����������
	struct SUpCost
	{
		UINT32 id;							//��ƷID
		BYTE type;							//��Ʒ����
		BYTE num;							//��Ʒ����
	};

	// �������Լӳ�
	struct SAttributes
	{
		BYTE type;							//��������
		UINT32 attrVal;						//���Լӳ�ֵ
	};

	// ���겿������
	struct ChainSoulPosConfig
	{
		std::vector<SUpCost> cost;			//��������
		std::vector<SAttributes> attr_plus;	//���Լӳ�
		UINT16 next_id;						//��һ��id
	};

	// ������Ĳ�������
	struct ChainSoulCoreConfig
	{
		BYTE condition_level;				//��������
		std::vector<SAttributes> attr_plus;	//���Լӳ�
	};

	// ����id->����
	typedef std::map<UINT16, ChainSoulPosConfig> cspc_map;

public:
	GreedTheLiveCfg();
	~GreedTheLiveCfg();

public:
	//��������
	BOOL Load(const char* pszFile);

	//�Ƿ�����
	bool IsOpen() const;

	//��ȡ��������
	bool GetChainSoulPosConfig(BYTE type, BYTE level, ChainSoulPosConfig *&out_config);

	//��ȡʥ���������
	bool GetChainSoulCoreConfig(BYTE level, ChainSoulCoreConfig *&out_config);

	//����������ȡ���ĵȼ�
	BYTE GetCoreLevelByConditionLevel(BYTE condition_level);

private:
	bool m_is_open;
	std::map<BYTE, cspc_map> m_chainSoulPosConfig;
	std::map<BYTE, ChainSoulCoreConfig> m_chainSoulCoreConfig;
	std::map<BYTE, std::vector<UINT16> > m_typeSet;
	std::map<BYTE, BYTE> m_conditionCorrespondsCore;
};

#endif