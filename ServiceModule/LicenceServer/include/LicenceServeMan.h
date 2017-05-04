#ifndef __LICENCE_SERVE_MAN_H__
#define __LICENCE_SERVE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
·þÎñÆô¶¯Í£Ö¹
*/

#include "LicenceSession.h"

class LicenceServeMan
{
	LicenceServeMan(const LicenceServeMan&);
	LicenceServeMan& operator = (const LicenceServeMan&);

public:
	LicenceServeMan();
	~LicenceServeMan();

	void Start();
	void Stop();

private:
	ICyaTcpServeMan* m_serveMan;
	LicenceSessionMaker m_sessionMaker;
};

void StartLicenceServe();
void StopLicenceServe();
LicenceServeMan* GetLicenceServeMan();


#endif