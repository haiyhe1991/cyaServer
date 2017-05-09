#ifndef __LINK_SERVER_H__
#define __LINK_SERVER_H__

#include "LinkSession.h"
#include "SessionMaker.h"
#include "TokenMan.h"
#include "MsgProcess.h"

class LinkServer
{
public:
	LinkServer();
	~LinkServer();

	int  InitLinkServer();
	void UnitLinkServer();

	/// Æô¶¯¡¢Í£Ö¹
	int  StartLinkServer();
	void StopLinkServer();

	bool ServerStarted() const;
	int GetSessionsCount();
private:
	inline bool ValidMaker();
	inline bool ValidMan();
private:
	SessionMaker* m_maker;
	ICyaTcpServeMan* m_man;
	bool m_started;		/// ±£´æServerManÆô¶¯×´Ì¬
	MsgProcess* m_proc;
	TokenMan* m_tokenman;
};

#endif