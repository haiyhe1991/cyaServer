#ifndef __DAILY_SIGN_CFG_H__
#define __DAILY_SIGN_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

//ÿ��ǩ��

class DailySignCfg
{
public:
	// ���߽���
	struct SDailySignAward
	{
		UINT32 id;									//��ƷID
		BYTE type;									//��Ʒ����
		UINT32 num;									//��Ʒ����
	};

	struct SDailySignItem
	{
		BYTE double_vip_level;						//˫����������vip�ȼ�
		bool continuous;							//�Ƿ�����
		std::vector<SDailySignAward> award;			//����
	};

	// ÿ��ǩ������
	struct SDailySignConfig
	{
		bool is_open;								//�Ƿ���
		LTMSEL start_time;							//��ʼʱ��
		LTMSEL end_time;							//����ʱ��
		std::map<BYTE, SDailySignItem> data;
	};


public:
	DailySignCfg();
	~DailySignCfg();

public:
	//��������
	BOOL Load(const char* pszFile);

	//�Ƿ���Ч
	bool IsDailySignValid() const;

	//��ȡ����
	bool GetDailySignConfig(BYTE days, const SDailySignItem *&config);

private:
	SDailySignConfig m_signConfig;
};

#endif