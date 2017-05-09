#ifndef __LINK_GLOBAL_OBJ_H__
#define __LINK_GLOBAL_OBJ_H__

#include "GWSSDK.h"

class LinkServer;
class LinkConfig;
class LicenceClient;

#define MAX_LINKER_CLIENT_NUM 5000

/// 初始化Config->LS->GWS->LinkServer

/// 初始化LinkServer
int InitLinkServer();
/// 初始化验证客户端
int InitLSLink();
/// 初始化配置
int InitConfig();

void UnitLinkServer();
void UnitLSLink();
void UnitConfig();

LinkServer* GlobalGetLinkServer();

LinkConfig* GlobalGetLinkConfig();

LicenceClient* GlobalGetLicenceClient();

#endif