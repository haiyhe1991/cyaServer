#include "GuildMan.h"
#include "DBSClient.h"
#include "RoleInfos.h"
#include "ConfigFileMan.h"
#include "cyaMaxMin.h"

static GuildMan* sg_guildMan = NULL;
void InitGuildMan()
{
	ASSERT(sg_guildMan == NULL);
	sg_guildMan = new GuildMan();
	ASSERT(sg_guildMan != NULL);
}

GuildMan* GetGuildMan()
{
	return sg_guildMan;
}

void DestroyGuildMan()
{
	GuildMan* guildMan = sg_guildMan;
	sg_guildMan = NULL;
	if (guildMan)
		delete guildMan;
}

GuildMan::GuildMan()
{

}

GuildMan::~GuildMan()
{

}

int GuildMan::QueryGuildName(UINT32 guildId, SQueryGuildNameRes* pNameRes)
{
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select name from guild where id=%u", guildId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT32 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		return MLS_GUILD_NOT_EXIST;
	}

	DBS_ROW row = DBS_FetchRow(result);
	ASSERT(row != NULL);
	pNameRes->guildId = guildId;
	strncpy(pNameRes->guildName, row[0], min(sizeof(pNameRes->guildName) - sizeof(char), strlen(row[0])));
	DBS_FreeResult(result);
	return retCode;
}