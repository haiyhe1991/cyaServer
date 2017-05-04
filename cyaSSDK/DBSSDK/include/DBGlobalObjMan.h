#ifndef __DB_GLOBAL_OBJ_MAN_H__
#define __DB_GLOBAL_OBJ_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "DBSSession.h"

class DBGlobalObjMan
{
private:
	DBGlobalObjMan(const DBGlobalObjMan&);
	DBGlobalObjMan& operator = (const DBGlobalObjMan&);

public:
	DBGlobalObjMan();
	~DBGlobalObjMan();

	ICyaTcpServeMan* GetDBSSessionMan()
	{
		return m_dbsSessionMan;
	}

private:
	DBSSessionMaker m_dbsSessionMaker;
	ICyaTcpServeMan* m_dbsSessionMan;
};

void InitDBGlobalObjMan();
void DestroyDBGlobalObjMan();
DBGlobalObjMan* GetDBGlobalObjMan();

#endif