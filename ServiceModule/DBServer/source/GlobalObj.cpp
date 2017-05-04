#include "GlobalObj.h"
#include "DBConfig.h"
#include "DBServerMan.h"
#include "MsgProcess.h"

INT InitializeDBSConfig()
{
	return GetDBConfigObj().InitializeConfig() ? 0 : -1;
}

INT InitializeDBServer()
{
	GetMsgProObj().InitializeMsgPro();
	return GetDBServerManObj().InitializeDBS() ? 0 : -1;
}

void FinishDBSConfig()
{
	GetDBConfigObj().FinishConfig();
}

void FinishDBServer()
{
	GetDBServerManObj().FinishDBS();
	GetMsgProObj().FinishMsgPro();
}

