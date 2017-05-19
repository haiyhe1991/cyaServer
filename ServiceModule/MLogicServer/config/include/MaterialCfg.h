#ifndef __MATERIAL_CFG_H__
#define __MATERIAL_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��������
*/
#include <map>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class MaterialCfg
{
public:
	MaterialCfg();
	~MaterialCfg();

#pragma pack(push, 1)
	struct SMaterialAttr
	{
		BYTE   level;			//���ϵȼ�
		BYTE   limitRoleLevel;	//���ƽ�ɫ�ȼ�(��ɫ�ȼ�С�ڸ�ֵ��Ӧ���и���Ʒ)
		UINT64 sellPrice;	//�ۼ�
		//UINT64 backPrice;	//�ع���
		//UINT64 shopSellPrice;	//�̵��ۼ�
		BYTE   bindMode;	//��ģʽ
		BYTE   maxStackCount;	//���ѵ���
		BYTE   quality;		//Ʒ��
		std::vector<UINT32> funcValVec;	//���Ϲ���ֵ
		SMaterialAttr()
		{
			level = 0;
			limitRoleLevel = 0;
			sellPrice = 0;
			//backPrice = 0;
			bindMode = 0;
			maxStackCount = 0;
			quality = 0;
			funcValVec.clear();
		}
	};
#pragma pack(pop)

public:
	//������Ʒ�����ļ�
	BOOL Load(const char* filename);
	//��ȡ��������
	int GetMaterialPropAttr(UINT16 materiaId, SPropertyAttr& propAttr, BYTE& maxStackCount);
	//��ȡ��Ʒ���ѵ�����
	BYTE GetMaterialMaxStack(UINT16 materiaId);
	//��ȡ�����û����ۼ۸�(����)
	int GetMaterialUserSellPrice(UINT16 materiaId, UINT64& price);
	//��ȡ���Ϲ���ֵ
	int GetMaterialFuncValue(UINT16 materiaId, UINT32& funcVal);
	//��ȡ���Ϲ���ֵ
	int GetMaterialFuncValueVec(UINT16 materiaId, std::vector<UINT32>& funcValVec);

private:
	std::map<UINT16/*��Ʒid*/, SMaterialAttr> m_materialAttrCfg;	//��Ʒ���ñ�
};

#endif