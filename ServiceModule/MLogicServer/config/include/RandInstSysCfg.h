#ifndef __RAND_INST_SYS_CFG_H__
#define __RAND_INST_SYS_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
�������ϵͳ����
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class RandInstSysCfg
{
public:
	RandInstSysCfg();
	~RandInstSysCfg();

#pragma pack(push, 1)
	struct SRandInstSysParam
	{
		BYTE isOpen;					//�Ƿ���
		BYTE chance;					//����
		UINT32 timerInterVal;			//ˢ��ʱ����
		UINT16 maxNumPerTransfer;		//ÿ����������������
		UINT16 maxOpenTransferNum;		//�������������
		UINT16 minOpenTransferNum;		//��С������������
		UINT32 openDurationTime;		//��������ʱ��

		BYTE startHour;					//ÿ�տ���ʱ��(Сʱ)
		BYTE startMin;					//ÿ�տ���ʱ��(����)
		BYTE endHour;					//ÿ�ս���ʱ��(Сʱ)
		BYTE endMin;					//ÿ�ս���ʱ��(����)

		std::vector<UINT16> transferVec;	//����������
	};
#pragma pack(pop)

public:
	//��������
	BOOL Load(const char* filename);
	//��ȡ�������ϵͳ����
	BOOL GetRandInstSysParam(SRandInstSysParam& param);

private:
	std::string m_filename;
};

#endif