#include "GMLogicProcMan.h"
#include "GWSClient.h"
#include "RolesInfoMan.h"
#include "RefreshDBMan.h"

#define ADD_GM_PROC_HANDLER(cmdCode, fun) \
do \
	{	\
	m_procHandler.insert(std::make_pair(cmdCode, (fnGMProcHandler)&GMLogicProcMan::fun));	\
	} while (0)

static GMLogicProcMan* sg_GMLogicMan = NULL;
void InitGMLogicProcMan()
{
	ASSERT(sg_GMLogicMan == NULL);
	sg_GMLogicMan = new GMLogicProcMan();
	ASSERT(sg_GMLogicMan != NULL);
}

GMLogicProcMan* GetGMLogicProcMan()
{
	return sg_GMLogicMan;
}

void DestroyGMLogicProcMan()
{
	GMLogicProcMan* pGMLogicMan = sg_GMLogicMan;
	sg_GMLogicMan = NULL;
	if (pGMLogicMan != NULL)
		delete pGMLogicMan;
}

GMLogicProcMan::GMLogicProcMan()
{

}

GMLogicProcMan::~GMLogicProcMan()
{

}

int GMLogicProcMan::ProcessLogic(GWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	int retCode = MLS_OK;
	std::map<UINT16, fnGMProcHandler>::iterator it = m_procHandler.find(cmdCode);
	if (it != m_procHandler.end())
		retCode = (this->*(it->second))(gwsSession, pHead, pData, nDatalen);
	else
		gwsSession->SendBasicResponse(pHead, MLS_UNKNOWN_CMD_CODE, cmdCode);
	return retCode;
}

int GMLogicProcMan::OnQueryPlayerActor(GWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	return 0;
}