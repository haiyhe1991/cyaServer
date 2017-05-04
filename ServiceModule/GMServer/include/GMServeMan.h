#ifndef __CGM_SERVE_MAN_H__
#define __CGM_SERVE_MAN_H__

#include "GMSession.h"

class CGMServeMan
{
public:
	CGMServeMan();
	~CGMServeMan();

	ICyaTcpServeMan* GetGMServeMan()
	{
		return m_gmserveMan;
	}

private:
	CGMSessionMaker m_gmsessionMaker;
	ICyaTcpServeMan* m_gmserveMan;
};


void InitGMServeManObj();

CGMServeMan* GetGMServeManObj();

void DestroyGMServeManObj();

#endif