#ifndef __CONFIG_FILE_MAN_H__
#define __CONFIG_FILE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�����ļ�������
*/

#define SGS_CASH_TABLE_NUM 5
#define SGS_CASH_TABLE_NAME_PREFIX "cash_"
#define SGS_CASHLIST_TABLE_NUM 5
#define SGS_CASHLIST_TABLE_NAME_PREFIX "cashlist_"

#define CASH_SAVE_TYPE_TO_ACTOR  1
#define CASH_SAVE_TYPE_TO_CASHDB  2

#include "cyaTypes.h"

class ConfigFileMan
{
	ConfigFileMan(const ConfigFileMan&);
	ConfigFileMan& operator = (const ConfigFileMan&);

public:
	ConfigFileMan();
	~ConfigFileMan();

	BOOL LoadConfigFile(const char* pszFilePath);

	const char* GetGwsIp() const
	{
		return m_gwsIp.c_str();
	}

	int GetGwsPort() const
	{
		return m_gwsPort;
	}

	int GetInitDBConnections() const
	{
		return m_initDBConnections;
	}

	int GetMaxDBConnections() const
	{
		return m_maxDBConnections;
	}

	int GetInitGWConnections() const
	{
		return m_initGWConnections;
	}

	const char* GetDBSIp() const
	{
		return m_dbsIp.c_str();
	}

	int GetDBSPort() const
	{
		return m_dbsPort;
	}

	int GetRefreshDBInterval() const
	{
		return m_refreshDBInterval;
	}

	const char* GetDBName() const
	{
		return m_dbName.c_str();
	}

	int GetMaxUnReadEmailSaveDays() const
	{
		return m_maxUnReadEmailSaveDays;
	}

	int GetMaxReadEmailSaveDays() const
	{
		return m_maxReadEmailSaveDays;
	}

	const char* GetVersion() const
	{
		return m_version.c_str();
	}

	BOOL IsInnerDebugMode() const
	{
		return m_isInnerDebug;
	}

	int GetSyncInstRankTime() const
	{
		return m_syncInstRankTime;
	}

	int GetSyncInstHistoryTime() const
	{
		return m_syncInstHistoryTime;
	}

	int GetMaxCacheItems() const
	{
		return m_maxCacheItems;
	}

	int GetSyncTaskHistoryTime() const
	{
		return m_syncTaskHistoryTime;
	}

	const char* GetGameCfgPath() const
	{
		return m_gameCfgPath.c_str();
	}

	BOOL IsCheckInstDropEmail() const
	{
		return m_instDropEmailChk;
	}

	int GetRechargeDBSPort() const
	{
		return m_rechargeDBSPort;
	}

	const char* GetRechargeDBSIp() const
	{
		return m_rechargeDBSIp.c_str();
	}

	const char* GetRechargeDBName() const
	{
		return m_rechargeDBName.c_str();
	}

	const char* GetPlayerActionDBName() const
	{
		return m_playerActionDBName.c_str();
	}

	int GetActionBatchNum() const
	{
		return m_actionBatchNum;
	}

	BOOL EnableCheckActorOrder() const
	{
		return m_enableCheckActorOrder;
	}

	//by hxw add 20150928 ��ȡ�����Լ�����ʱ��
	UINT16 GetPartID() const{ return m_partID; }
	std::string GetPartName() const { return m_partName; }
	LTMSEL GetPartOpenTime() const { return m_partStartTime; }
	//end

	//add by hxw 20151111 ������Ҵ������ݿ�
	int GetCashDBSPort() const
	{
		return m_cashDBSPort;
	}

	const char* GetCashDBSIp() const
	{
		return m_cashDBSIp.c_str();
	}

	const char* GetCashDBName() const
	{
		return m_cashDBName.c_str();
	}

	//add by hxw 20151116 ���Ӵ��Ҵ洢����
	int GetCashSaveType() const
	{
		return m_cashSaveType;
	}

	//add by hxw 20151210 ���� �Ƿ����߰汾
	BOOL IsRelease() const
	{
		return m_release == 1;
	}

private:
	int m_cashDBSPort;			//��ֵ���ݿ�˿�
	std::string m_cashDBSIp;	//��ֵ���ݿ�ip
	std::string m_cashDBName;	//��ֵ���ݿ���
	//end by 20151111

private:
	std::string m_gwsIp;	//����ip
	int m_gwsPort;			//����port
	std::string m_version;	//�汾
	std::string m_gameCfgPath;	//��Ϸ�����ļ�·��

	std::string m_dbsIp;	//���ݷ�����ip
	int m_dbsPort;			//���ݷ�����port
	std::string m_dbName;	//���ݿ���
	int m_initDBConnections;	//��ʼ�����ݷ�����������
	int m_maxDBConnections;		//������ݷ�����������

	int m_initGWConnections;	//��ʼ�����ط�����������
	int m_refreshDBInterval;	//ˢ�½�ɫ������Ϣʱ����

	int m_maxCacheItems;			//��󻺴�����
	int m_maxUnReadEmailSaveDays;	//δ���ʼ���󱣴�����
	int m_maxReadEmailSaveDays;		//�Ѷ��ʼ���󱣴�����
	BOOL m_isInnerDebug;			//�Ƿ��ڲ�����ģʽ

	int m_syncInstRankTime;		//ˢ�¸�������ʱ��
	int m_syncInstHistoryTime;	//ˢ�¸�����ʷ��¼ʱ��
	int m_syncTaskHistoryTime;	//ˢ��������ʷ��¼ʱ��

	BOOL m_instDropEmailChk;	//�Ƿ��⸱�������ʼ�

	int m_rechargeDBSPort;			//��ֵ���ݿ�˿�
	std::string m_rechargeDBSIp;	//��ֵ���ݿ�ip
	std::string m_rechargeDBName;	//��ֵ���ݿ���
	std::string m_playerActionDBName;	//�����Ϊ���ݿ���
	int m_actionBatchNum;
	BOOL m_enableCheckActorOrder;
	//20150928 add by hxw ������Ϣ��¼
	LTMSEL m_partStartTime;
	std::string m_partName;
	UINT16 m_partID;
	BYTE m_cashSaveType;
	BYTE m_release;	//�Ƿ������߰汾
	//end
};

BOOL InitConfigFileMan(const char* pszFilePath);
ConfigFileMan* GetConfigFileMan();
void DestroyConfigFileMan();
#endif