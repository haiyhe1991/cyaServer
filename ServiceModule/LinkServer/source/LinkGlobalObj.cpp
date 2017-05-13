#include "LinkGlobalObj.h"
#include "cyaLog.h"
#include "cyaIpCvt.h"

#include "MsgProcess.h"
#include "LinkServer.h"
#include "LinkConfig.h"
#include "LicenceClient.h"

#include "CyaObject.h"

DELAR_GLOBAL_OBJ(LinkServer)
DELAR_GLOBAL_OBJ(LinkConfig)
DELAR_GLOBAL_OBJ(LicenceClient)

int InitLinkServer()
{
	NEW_FETCH_OBJ_EX(LinkServer)
		return GET_GLOBAL_OBJ(LinkServer)->InitLinkServer();
}

int InitLSLink()
{
	VALID_GLOBAL_OBJ(LinkConfig)
		NEW_FETCH_OBJ(LicenceClient)
		VALID_GLOBAL_OBJ(LicenceClient)
		// 	if (0 == GET_GLOBAL_OBJ(LicenceClient)->ConnectLicenceServer())
		// 	{
		// 		return GET_GLOBAL_OBJ(LicenceClient)->LogInLicenceServer();
		// 	}

		return GET_GLOBAL_OBJ(LicenceClient)->ConnectLicenceServer();
}

int InitConfig()
{
	NEW_FETCH_OBJ_EX(LinkConfig)
		return GET_GLOBAL_OBJ(LinkConfig)->InitConfig("./cyaLinkServer.properties");
}

void UnitLinkServer()
{
	if (GET_GLOBAL_OBJ(LinkServer))
	{
		//GET_GLOBAL_OBJ(LinkServer)->UnitLinkServer();
		DELETE_FETCH_OBJ(LinkServer)
	}
}

void UnitLSLink()
{
	DELETE_FETCH_OBJ_EX(LicenceClient)
}


void UnitConfig()
{
	DELETE_FETCH_OBJ_EX(LinkConfig)
}

LinkServer* GlobalGetLinkServer()
{
	return GET_GLOBAL_OBJ(LinkServer);
}

LinkConfig* GlobalGetLinkConfig()
{
	return GET_GLOBAL_OBJ(LinkConfig);
}

LicenceClient* GlobalGetLicenceClient()
{
	return GET_GLOBAL_OBJ(LicenceClient);
}