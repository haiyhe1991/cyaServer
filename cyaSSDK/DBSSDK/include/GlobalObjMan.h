#ifndef __GLOBAL_OBJ_MAN_H__
#define __GLOBAL_OBJ_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "DBSSession.h"

class GlobalObjMan
{
private:
	GlobalObjMan(const GlobalObjMan&);
	GlobalObjMan& operator = (const GlobalObjMan&);

public:
	GlobalObjMan();
	~GlobalObjMan();

	ICyaTcpServeMan* GetDBSSessionMan()
	{
		return m_dbsSessionMan;
	}

private:
	DBSSessionMaker m_dbsSessionMaker;
	ICyaTcpServeMan* m_dbsSessionMan;
};

void InitGlobalObjMan();
void DestroyGlobalObjMan();
GlobalObjMan* GetGlobalObjMan();

#endif