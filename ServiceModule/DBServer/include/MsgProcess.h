#ifndef __MSG_PROCESS_H__
#define __MSG_PROCESS_H__

#include "cyaTypes.h"

#include <map>
#include <vector>

#include "cyaLocker.h"
#include "cyaRefCount.h"
#include "cyaCoreTimer.h"
#include "cyaCore.h"

#include "DBServerMan.h"
#include "TcpSession.h"
#include "SqlSingle.h"
#include "DBTable.h"

static const DWORD MAX_COM_RES_LEN = 1024;			///����SDBSResultRes��BUFF����
static const DWORD CLEAR_SESSION_IDLE = 20 * 1000 + 1;	///sessionû�з����������ж�Ϊ���ߣ�����

#define ENABLE_HEART_BEAT	0

class MsgProcess
{
public:
	MsgProcess();
	~MsgProcess();

	BOOL InitializeMsgPro();
	void FinishMsgPro();

	/// TcpSession ��Ϣ����
	INT MessageProxy(DBTcpSession* sessionSit, const void* msgData, INT msgLen);
	/// ��������TcpSession
	INT SessionRegist(DBTcpSession* sessionSit);
	void SessionUnRegist(DBTcpSession* sessionSit);
private:
	typedef INT(MsgProcess::*OnCmd)(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	typedef std::map<DBTcpSession*, LTMSEL> HeartSessinMap;
	typedef std::map<INT, OnCmd> CmdHandleMap;

	INT OnMsgRequest(DBTcpSession* mySession, UINT16 cmdType, const void* msgData, UINT dataLen);
	void OnInitCmdHandle();
	CmdHandleMap::iterator FindCmdHandle(INT cmdType);

	typedef UINT64(SqlSingle::*SqlRowAffect)(const char* strCmd, DWORD CmdLen);
	INT OnDbCommonAffect(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen, SqlRowAffect affectFun);
private:
	/// ��Mysql������Load���ڴ����ݿ�
	INT OnMemLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// ���ڴ�����ͬ����Mysql
	INT OnMemUnLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// ��ȡ������¼��ȫ���ֶ�
	INT OnMemGet(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// �޸�һ����¼�Ķ���ֶ�
	INT OnMemSet(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// ����һ����¼�Ķ���ֶ�
	INT OnMemAppend(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// ֻɾ��һ����¼
	INT OnMemRemove(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);

	INT OnDbQuery(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	//// ����ĺ�������Ӱ�������
	INT OnDbUpdate(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	INT OnDbInsert(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	INT OnDbDelete(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);

	INT OnSessionHeart(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
private:
	inline bool HeartTimeout(DWORD);
	static void HeartTimerCallback(void* param, TTimerID id);
	INT SessinClearThWorker();

	char* ParseDBName(const void* msgData, UINT dataLen);
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);
private:
	CyaCoreTimer m_heartTimer;

	CXTLocker m_handleLocker;
	CmdHandleMap m_cmdMap;
	HeartSessinMap m_heartMap;
};

MsgProcess& GetMsgProObj();

#endif