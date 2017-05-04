#ifndef __GW_SERVE_MAN_H__
#define __GW_SERVE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
·þÎñÆô¶¯Í£Ö¹
*/

#include "GWSession.h"

class GWServeMan
{
	GWServeMan(const GWServeMan&);
	GWServeMan& operator = (const GWServeMan&);

public:
	GWServeMan();
	~GWServeMan();

	void Start();
	void Stop();

private:
	ICyaTcpServeMan* m_serveMan;
	GWSessionMaker m_sessionMaker;
};

void StartGWServe();
void StopGWServe();


#endif