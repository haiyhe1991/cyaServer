#ifndef __LINK_GLOBAL_OBJ_H__
#define __LINK_GLOBAL_OBJ_H__

#include "GWSSDK.h"

class LinkServer;
class LinkConfig;
class LicenceClient;

#define MAX_LINKER_CLIENT_NUM 5000

/// ��ʼ��Config->LS->GWS->LinkServer

/// ��ʼ��LinkServer
int InitLinkServer();
/// ��ʼ����֤�ͻ���
int InitLSLink();
/// ��ʼ������
int InitConfig();

void UnitLinkServer();
void UnitLSLink();
void UnitConfig();

LinkServer* GlobalGetLinkServer();

LinkConfig* GlobalGetLinkConfig();

LicenceClient* GlobalGetLicenceClient();

#endif