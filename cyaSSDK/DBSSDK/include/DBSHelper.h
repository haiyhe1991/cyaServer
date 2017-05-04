#ifndef __DBS_HELPER_H__
#define __DBS_HELPER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include "cyaLocker.h"
#include "cyaTypes.h"
#include "cyaTcp.h"
#include "cyaRefCount.h"
#include "cyaCoreTimer.h"
#include "DBSProtocol.h"
#include "DBSSDK.h"

class DBSSession;
class DBSHelper
{
public:
	DBSHelper(const char* dbsIp,
		int dbsPort,
		unsigned int connTimeout/* = 5000*/,
		unsigned int heartbeatInterval /*= 10*1000*/,
		unsigned int writeTimeout/* = 0*/,
		unsigned int readTimeout/* = 0*/);
	~DBSHelper();

public:
	int  Connect();
	void DisConnect();

	int  ExcuteSQL(DBS_MSG_TYPE cmdType, const char* dbName, const char* sql, DBS_RESULT& result, int sqlLen = -1);
	int  Load(const char* dbName, const char* tableName, const char* sql, DBS_RESULT& result, int sqlLen = -1);
	int  Get(const char* dbName, const char* tableName, const SDBSValue* pValsArray, int arrCount, DBS_RESULT& result);
	int  Set(const char* dbName, const char* tableName, const char* keyFieldValue, const SDBSKeyValue* pKeyValueArray, int arrCount, DBS_RESULT& result);
	int  Append(const char* dbName, const char* tableName, const SDBSKeyValue* pKeyValueArray, int arrCount, DBS_RESULT& result);
	int  Remove(const char* dbName, const char* tableName, const SDBSKeyValue* pKeyVal, DBS_RESULT& result);
	int  UnLoad(const char* dbName, const char* tableName, DBS_RESULT& result);

	void NotifySessionBroken();
	void SetLocalSession(DBSSession* session);
	void InputResponseData(unsigned int seq, const void* pData, int dataBytes, BOOL isOver);

private:
	void  OnMakeDBSHeader(SDBSProtocolHead* pHeader, UINT16 nPayLoadBytes, DWORD seq, BOOL isOver);
	long  OnGetCmdSeq();
	int	  OnExcuteCmd(const BYTE* payload, int payloadBytes, BOOL needFree, DBS_RESULT& result);
	int   OnFillData(BYTE* pDest, int offset, const void* pSrc, int srcLen);

	static void ReconnectTimer(void* param, TTimerID id);
	void OnReconnect();

	static void HeartbeatTimer(void* param, TTimerID id);
	void OnHeartbeat();

private:
	int m_dbsPort;
	std::string m_dbsIp;
	DWORD m_connTimeout;
	DWORD m_writeTimeout;
	DWORD m_readTimeout;
	DWORD m_heartbeatInterval;
	BOOL  m_reconnect;

	class DataBlock : public BaseRefCount1
	{
	private:
		DataBlock(const DataBlock&);
		DataBlock& operator = (const DataBlock&);

	public:
		DataBlock()
		{
			Init();
		}

		~DataBlock()
		{
			Close();
		}

		virtual void DeleteObj()
		{
			CyaTcp_Delete(this);
		}

		BOOL Wait(DWORD timeout = INFINITE)
		{
			return OSSemaWait(&_sema, timeout);
		}

		void* DataPtr() const
		{
			return _data;
		}

		int Length() const
		{
			return _dataLen;
		}

		void PostSema()
		{
			OSSemaPost(&_sema);
		}

		void* Drop()
		{
			void* p = _data;
			_data = NULL;
			return p;
		}

		void Input(const void* pData, int dataBytes, BOOL isOver)
		{
			if (_data == NULL)
			{
				_data = (void*)pData;
				_dataLen = dataBytes;
			}
			else
			{
				ASSERT(_dataLen > 0);
				void* p = _data;
				int nSize = dataBytes + _dataLen;
				_data = CyaTcp_Alloc(nSize + 1);
				ASSERT(_data != NULL);
				memcpy(_data, p, _dataLen);
				memcpy((BYTE*)_data + _dataLen, pData, dataBytes);
				_dataLen = nSize;
				CyaTcp_Free(p);
			}

			ASSERT(_data != NULL && _dataLen > 0);
			if (isOver)
				OSSemaPost(&_sema);
		}

	private:
		void Init()
		{
			OSSemaInit(&_sema);
			_data = NULL;
			_dataLen = 0;
		}
		void Close()
		{
			OSSemaDestroy(&_sema);
			if (_data != NULL)
			{
				CyaTcp_Free(_data);
				_data = NULL;
			}
			_dataLen = 0;
		}

	private:
		OSSema _sema;
		void*  _data;
		int    _dataLen;
	};

	typedef std::map<DWORD, DataBlock*> DataBlockMap;
	DataBlockMap m_dataBlocks;
	CXTLocker m_locker;
	long m_seq;
	DBSSession* m_localSession;
	CyaCoreTimer m_reconnectTimer;
	CyaCoreTimer m_heartbeatTimer;
	DWORD m_recvHeartTick;
	char* m_heartbeatData;
	int m_heartbeatDataLen;
};


#endif