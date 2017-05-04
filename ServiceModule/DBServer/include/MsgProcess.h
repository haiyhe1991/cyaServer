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

static const DWORD MAX_COM_RES_LEN = 1024;			///返回SDBSResultRes的BUFF长度
static const DWORD CLEAR_SESSION_IDLE = 20 * 1000 + 1;	///session没有发送心跳，判断为掉线，清理

#define ENABLE_HEART_BEAT	0

class MsgProcess
{
public:
	MsgProcess();
	~MsgProcess();

	BOOL InitializeMsgPro();
	void FinishMsgPro();

	/// TcpSession 消息代理
	INT MessageProxy(DBTcpSession* sessionSit, const void* msgData, INT msgLen);
	/// 保存所有TcpSession
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
	/// 将Mysql的数据Load到内存数据库
	INT OnMemLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// 将内存数据同步到Mysql
	INT OnMemUnLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// 获取多条记录的全部字段
	INT OnMemGet(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// 修改一条记录的多个字段
	INT OnMemSet(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// 增加一条记录的多个字段
	INT OnMemAppend(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	/// 只删除一条记录
	INT OnMemRemove(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);

	INT OnDbQuery(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
	//// 下面的函数返回影响的行数
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