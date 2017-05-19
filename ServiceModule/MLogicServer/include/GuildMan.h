#ifndef __GUILD_MAN_H__
#define __GUILD_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�������
*/

#include "ServiceProtocol.h"

class GuildMan
{
	GuildMan(const GuildMan&);
	GuildMan& operator = (GuildMan&);

public:
	GuildMan();
	~GuildMan();

public:
	int QueryGuildName(UINT32 guildId, SQueryGuildNameRes* pNameRes);
};

void InitGuildMan();
GuildMan* GetGuildMan();
void DestroyGuildMan();

#endif