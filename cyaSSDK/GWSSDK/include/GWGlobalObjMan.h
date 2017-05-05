#ifndef __GW_GLOBAL_OBJ_MAN_H__
#define __GW_GLOBAL_OBJ_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "GWSClientSession.h"

class GWGlobalObjMan
{
private:
	GWGlobalObjMan(const GWGlobalObjMan&);
	GWGlobalObjMan& operator = (const GWGlobalObjMan&);

public:
	GWGlobalObjMan();
	~GWGlobalObjMan();

	ICyaTcpServeMan* GetGWSSessionMan()
	{
		return m_gwsSessionMan;
	}

private:
	GWSClientSessionMaker m_gwsSessionMaker;
	ICyaTcpServeMan* m_gwsSessionMan;
};

void InitGWGlobalObjMan();
void DestroyGWGlobalObjMan();
GWGlobalObjMan* GetGWGlobalObjMan();

#endif