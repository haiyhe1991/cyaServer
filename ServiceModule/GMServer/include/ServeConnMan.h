#ifndef __CSERVE_CONN_MAN_H__
#define __CSERVE_CONN_MAN_H__

#include "DBSSdk.h"

#include <vector>
#include <map>
#include "cyaLocker.h"

///Linker���Ӻ��socket ��key
struct LinkSocketKey
{
	SOCKET sock; //����linker��socket
	char key[33]; //��Կ(�շ����ݼӽ���)
};

typedef std::vector<LinkSocketKey/*socket��key��Ϣ*/> LKSocketKey; //Linker ������Ϣ(�������ӵ�socket�ͼ���key)
typedef std::map<UINT16/*��������ID*/, LKSocketKey/*socket��key����*/> PartLkSockKey; //����������Ϣ

class CServeConnMan
{
public:
	CServeConnMan();
	~CServeConnMan();

	int InitConn();
	void DestroyConn();

	///��ȡLink����������
	void GetLinkSocketKey(UINT16 platId/*ƽ̨ID*/, UINT16 partId/*����ID*/, PartLkSockKey& sockKey);
	///��ȡƽ̨�������ݿ����������
	void GetDBShandle(UINT16 platId/*ƽ̨ID*/, UINT16 partId/*����ID*/, std::map<UINT16/*����ID*/, DBSHandle/*�������ݿ�handle*/>& dbsHandle);

	///��ȡ��ֵ���ݿ����������
	void GetRechargeDBShandle(DBSHandle& handle)
	{
		handle = m_rechargeDBSHanle;
	}
	///��ȡ�ʺ����ݿ����������
	void GetAccountDBShandle(DBSHandle& handle)
	{
		handle = m_accountDBSHanle;
	}



private:
	///��ʼ��ƽ̨DBServer ����
	int InitPlatformDBServeHandle();
	///��ʼ��ƽ̨Linker ����
	int InitPlatformLinkerServeConn();
	///��ʼ����ֵDBServer ����
	int InitRechargeDBServeHandle();
	///��ʼ���ʺ�DBServer ����
	int InitAccountDBServeHandle();

	///����ƽ̨DBServer ����
	void DestroyPlatformDBServeHandle();
	///����ƽ̨Linker ����
	void DestroyPlatformLinkerServeConn();
	///���ٳ�ֵDBServer ����
	void DestroyRechargeDBServeHandle();
	///�����ʺ�DBServer ����
	void DestroyAccountDBServeHandle();


private:
	int  LoginLinker(const char* ip, int port, SOCKET* sockt, char key[33]);

private:
	typedef std::map<UINT16/*ƽ̨ID*/, PartLkSockKey/*ƽ̨linker socket��key*/> PlatLkSockKey; //ƽ̨����������Ϣ
	typedef std::map<UINT16/*��������ID*/, DBSHandle/*�������ݿ�Handle*/> PartDBSHandle;	//�������ݿ������������Ϣ
	typedef std::map<UINT16/*ƽ̨ID*/, PartDBSHandle/*ƽ̨���ݿ�Handle*/> PlatDBSHandle;	//ƽ̨�������ݿ������������Ϣ

	PlatLkSockKey m_platLkSocketKey; //ƽ̨��������Sockt��key
	PlatDBSHandle m_platDBShandle;   //ƽ̨�������ݿ������handle
	DBSHandle m_rechargeDBSHanle;    //��ֵ���ݿ������handle
	DBSHandle m_accountDBSHanle;    //�ʺ����ݿ������handle

	CXTLocker m_connLocker;
	CXTLocker m_dbconnLocker;


};

int InitServeConnObj();

CServeConnMan* GetServeConnObj();

void DestroyServeConnObj();

#endif