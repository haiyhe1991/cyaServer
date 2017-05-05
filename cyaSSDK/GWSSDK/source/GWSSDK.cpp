#include "GWSSDK.h"
#include "GWSHelper.h"
#include "GWGlobalObjMan.h"

GWSSDK_API void GWS_Init()
{
	InitGWGlobalObjMan();
}

GWSSDK_API GWSHandle GWS_FetchHandle(const char* gwsIp, int gwsPort,
	EServerType serverType,
	fnDataCallback cb, void* context,
	unsigned int connTimeout/* = 5000*/,
	unsigned int writeTimeout/* = 0*/,
	unsigned int readTimeout/* = 0*/)
{
	GWSHelper* pHelper = new GWSHelper(gwsIp, gwsPort, serverType, cb, context,
		connTimeout, writeTimeout, readTimeout);
	ASSERT(pHelper != NULL);
	return pHelper;
}

GWSSDK_API void GWS_SetEventCallback(GWSHandle handle, fnEventCallback cb, void* context)
{
	((GWSHelper*)handle)->SetEventCallback(cb, context);
}

GWSSDK_API int  GWS_Login(GWSHandle handle)
{
	return ((GWSHelper*)handle)->Login();
}

GWSSDK_API void  GWS_Logout(GWSHandle handle)
{
	((GWSHelper*)handle)->Logout();
}

GWSSDK_API int   GWS_SendDataPacket(GWSHandle handle, const void* data,
		int dataBytes, int targetId,
		unsigned int userId, ESGSPacketType pktType,
		int isCrypt, long* seq/* = NULL*/)
{
	return ((GWSHelper*)handle)->SendDataPacket(data, dataBytes, targetId, userId, pktType, isCrypt, seq);
}

GWSSDK_API int   GWS_SendDataPacketEx(GWSHandle handle, const SGWSProtocolHead* pHead, const void* data, int dataBytes)
{
	return ((GWSHelper*)handle)->SendDataPacketEx(pHead, data, dataBytes);
}

GWSSDK_API void GWS_ReleaseHandle(GWSHandle handle)
{
	((GWSHelper*)handle)->ReleaseHandle();
}

GWSSDK_API void GWS_Cleanup()
{
	DestroyGWGlobalObjMan();
}