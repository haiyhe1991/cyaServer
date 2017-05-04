#ifndef __GM_CFG_MAN_H__
#define __GM_CFG_MAN_H__

#include "cyaBase.h"
#include <map>
#include <vector>
#include "json.h"

struct LinkerServeinfo
{
	char lkIp[17]; //Linker IP
	UINT16 lkPort; //Linker �˿�
};

struct ServeInfo
{
	char sIp[17]; //IP
	UINT16 sPort; // �˿�
};

struct PlatInfo
{
	UINT16 channelid; //����ID
	char channelname[33]; //������
	std::vector<UINT16> partId; //��������
};

typedef std::vector<UINT16> PartIDVec;		//����;
typedef std::vector<LinkerServeinfo*> LinkerSInfoVec;
typedef std::vector<ServeInfo/*���ӷ�����IP�Ͷ˿���Ϣ*/> ServeInfoVec;
typedef std::map<UINT16/*����ID*/, ServeInfoVec/*����������ip�Ͷ˿���Ϣ*/> LKServeInfo;
typedef std::map<UINT16/*��������*/, LKServeInfo/*����linker��Ϣ*/> ChannelLinkerServeInfo;
typedef std::map<UINT16/*��������*/, ServeInfo/*DB������IP�Ͷ˿���Ϣ*/> DBSServeInfo;
typedef std::map<UINT16/*��������*/, DBSServeInfo/*����DB��������Ϣ*/> ChannelDBServeInfo;
//typedef std::map<UINT16/*ƽ̨ID*/,ChannelDBServeInfo/*����*/> Platinfo;	//��ʱû����Ҫ;

class CGMCfgMan
{
public:
	CGMCfgMan();
	~CGMCfgMan();

	///��������
	int Load(const char* cfgName);

	///��ȡ�������˿ں�
	UINT16 GetServePort()
	{
		return m_servePort;
	}

	///��ȡ����ˢ�¼��ʱ��(��λ����)
	UINT16 GetRefreshTime()
	{
		return m_onlineRefreshTime;
	}
	///��ȡʵʱ����ʱ����(��λ:��)
	UINT16 GetRefrNewTime()
	{
		return m_realNewTime;
	}

	///��ȡ�������ʱ��(��λ����)
	UINT16 GetHeartbeatInterval()
	{
		return m_heartbeatInterval;
	}
	///��ȡ���ݿ����ʱ��;
	char* GetdbsTime()
	{
		return dbstime;
	}

	///�Ƿ�д�����ݿ�
	BOOL IsWriteIntoDb()
	{
		return m_isWriteIntoDb;
	}

	///�Ƿ����
	BOOL IsTest()
	{
		return m_isTest;
	}

	///��ȡ��������
	std::vector<UINT16>& GetLeaveKeepTimeType()
	{
		return m_leaveKeepType;
	}


	//��ȡ��������������������Ϣ
	ChannelLinkerServeInfo& GetPlatLinkerServeInfo()
	{
		return m_channelLkServeInfo;
	}

	//��ȡ�����������ݿ������������Ϣ
	ChannelDBServeInfo& GetPlatDBServeInfo()
	{
		return m_channelDbServeInfo;
	}

	//��ȡ��ֵ���ݿ������������Ϣ
	ServeInfo& GetRechargeDBServeInfo()
	{
		return m_rechargeDBs;
	}

	//��ȡ�ʺ����ݿ������������Ϣ
	ServeInfo& GetAccountDBServeInfo()
	{
		return m_accountDBs;
	}

	//��ȡ����ƽ̨/*ƽ̨����IOS�밲׿ƽ̨*/;
	std::vector<UINT16/*ƽ̨ID*/>& GetAllPlatId()
	{
		return m_platId;
	}
	// ��ȡ����id
	std::vector<UINT16/*����ID*/>& GetAllchannel()
	{
		return m_channel;
	}
	//��ȡָ�������ķ���
	void GetPlatAllPartId(UINT16 platId, std::vector<UINT16>& partVec)
	{
		partVec.clear();
		std::map<UINT16, PartIDVec>::iterator it = m_channelPartId.find(platId);
		if (it != m_channelPartId.end())
			partVec = it->second;
	}

	///��ȡ����������Ϣ
	std::vector<PlatInfo>& GetPlatInfo()
	{
		return m_channelpartInfo;
	}

private:
	BOOL CheckFile(const char* cfgName, Json::Value& jvalue);

	///�����������ݿ��������Ϣ
	int ParseNewDBSCfgMan(Json::Value& jvalue);
	///��������Linker��������Ϣ
	int ParseNewLKSCfgMan(Json::Value& jvalue);
	///������ֵ���ݿ��������Ϣ
	int ParseNewRechargeDBSCfgMan(Json::Value& jvalue);
	///�����ʺ����ݿ��������Ϣ
	int ParseNewAccountDBSCfgMan(Json::Value& jvalue);

	inline void NewObj(LinkerServeinfo*& p)
	{
		p = new LinkerServeinfo();
	}

private:

	std::map<std::string, int> m_dbsInfo;
	std::map<UINT16, LinkerSInfoVec> m_lksInfo;
	std::vector<UINT16> m_leaveKeepType;			//����ʱ������
	std::vector<UINT16> m_platId;					//ƽ̨ID��������������������;
	std::vector<UINT16> m_channel;					//����ID;
	std::map<UINT16, PartIDVec> m_channelPartId;	//�������� 
	std::vector<PlatInfo> m_channelpartInfo;		//����������Ϣ


	UINT16 m_servePort;		  //����������˿�
	UINT16 m_heartbeatInterval; //�������ʱ��[��λ:��]
	UINT16 m_onlineRefreshTime; //��������ˢ�¼��ʱ��[��λ:��]
	UINT16 m_realNewTime;		//ʵʱ����[��λ:��];
	BOOL m_isWriteIntoDb;       //�Ƿ�д�����ݿ�
	BOOL m_isTest;			  //�Ƿ����
	char dbstime[20];		  //���ݿ����ʱ��;

	//Platinfo			   m_platinfo;						//ƽ̨����DBServer��Ϣ;
	ChannelLinkerServeInfo m_channelLkServeInfo;			//��������Linker��Ϣ
	ChannelDBServeInfo m_channelDbServeInfo;				//��������DBServer��Ϣ
	ServeInfo m_rechargeDBs;						//��ֵ���ݿ��������Ϣ
	ServeInfo m_accountDBs;						//�ʺ����ݿ��������Ϣ





};

void InitCfgManObj(const char* cfgName);
CGMCfgMan* GetCfgManObj();
void DestroyCfgManObj();

#endif