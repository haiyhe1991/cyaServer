#include "gb_utf8.h"
#include "InstOccupyNotify.h"
#include "GameCfgFileMan.h"
#include "RolesInfoMan.h"
#include "EmailCleanMan.h"
#include "ServiceErrorCode.h"
#include "GWSClientMan.h"

static InstOccupyNotify* sg_occupyNotify = NULL;
void InitInstOccupyNotify()
{
	ASSERT(sg_occupyNotify == NULL);
	sg_occupyNotify = new InstOccupyNotify();
	ASSERT(sg_occupyNotify != NULL);
	sg_occupyNotify->Start();
}

InstOccupyNotify* GetInstOccupyNotify()
{
	return sg_occupyNotify;
}

void DestroyInstOccupyNotify()
{
	InstOccupyNotify* pOccupyNotify = sg_occupyNotify;
	sg_occupyNotify = NULL;
	if (pOccupyNotify != NULL)
	{
		pOccupyNotify->Stop();
		delete pOccupyNotify;
	}
}

InstOccupyNotify::InstOccupyNotify()
{

}

InstOccupyNotify::~InstOccupyNotify()
{

}

void InstOccupyNotify::Start()
{
	m_sendEmailTimer.Start(SendInstOccupyEmailTimer, this, 10 * 1000);
}

void InstOccupyNotify::Stop()
{
	m_sendEmailTimer.Stop();

	CXTAutoLock lock(m_locker);
	for (std::list<SInstOccupyEmail>::iterator it = m_instOccupyEmailList.begin(); it != m_instOccupyEmailList.end(); ++it)
		SendInstOccupyEmail(*it);

	m_instOccupyEmailList.clear();
}

void InstOccupyNotify::SendInstOccupyEmail(const std::list<SInstOccupyEmail>& emails)
{
	CXTAutoLock lock(m_locker);
	std::list<SInstOccupyEmail>::const_iterator it = emails.begin();
	for (; it != emails.end(); ++it)
		m_instOccupyEmailList.push_back(*it);
}

void InstOccupyNotify::SendInstOccupyNotify(BYTE curUserLinkerId, UINT32 curUserId, char* buf, int nLen, std::list<SInstOccupyEmail>& emails)
{
	for (std::list<SInstOccupyEmail>::iterator it = emails.begin(); it != emails.end(); ++it)
	{
		if (it->type == EINST_OCCUPY_BY_NEW_ROLE)
		{
			GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
			if (gwsSession != NULL)
			{
				//占领者
				gwsSession->SendResponse(curUserLinkerId, curUserId, MLS_OK, MLS_INST_OCCUPY_PUSH_NOTIFY, buf, nLen);
				//被占领者
				BYTE linkerId = 0;
				int retCode = GetRolesInfoMan()->GetUserLinkerId(it->occupiedUserId, linkerId);
				if (retCode == MLS_OK && linkerId > 0)
					gwsSession->SendResponse(linkerId, it->occupiedUserId, MLS_OK, MLS_INST_OCCUPY_PUSH_NOTIFY, buf, nLen);
			}

			break;
		}
		else if (it->type == EINST_FIRST_OCCUPY)
		{
			GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
			if (gwsSession != NULL)
				gwsSession->SendResponse(curUserLinkerId, curUserId, MLS_OK, MLS_INST_OCCUPY_PUSH_NOTIFY, buf, nLen);

			emails.erase(it);
			break;
		}
	}
}

void InstOccupyNotify::SendInstOccupyEmailTimer(void* param, TTimerID id)
{
	InstOccupyNotify* pThis = (InstOccupyNotify*)param;
	if (pThis != NULL)
		pThis->OnSendInstOccupyEmail();
}

void InstOccupyNotify::OnSendInstOccupyEmail()
{
	if (m_instOccupyEmailList.empty())
		return;

	std::list<SInstOccupyEmail> emails;
	{
		CXTAutoLock lock(m_locker);
		emails.assign(m_instOccupyEmailList.begin(), m_instOccupyEmailList.end());
		m_instOccupyEmailList.clear();
	}

	for (std::list<SInstOccupyEmail>::iterator it = emails.begin(); it != emails.end(); ++it)
		SendInstOccupyEmail(*it);
}

void InstOccupyNotify::SendInstOccupyEmail(const SInstOccupyEmail& occupyEmail)
{
	std::string instName;
	int retCode = GetGameCfgFileMan()->GetInstName(occupyEmail.instId, instName);
	if (retCode != MLS_OK)
		return;

	char szGBOccupierNick[MAX_UTF8_NICK_LEN] = { 0 };	//占领者昵称
	utf8_gb2312(occupyEmail.occupierNick.c_str(), (int)occupyEmail.occupierNick.length(), szGBOccupierNick, sizeof(szGBOccupierNick)-sizeof(char));

	char szContent[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
	EmailCleanMan::SSyncEmailSendContent email;
	email.pGBSenderNick = "系统";
	email.pGBCaption = "副本占领";
	email.receiverRoleId = occupyEmail.occupiedRoleId;
	email.receiverUserId = occupyEmail.occupiedUserId;

	if (occupyEmail.type == EINST_OCCUPY_BY_NEW_ROLE)		//被新的玩家占领
	{
		sprintf(szContent, "玩家%s完全无视你的存在, 很轻松的就把你的%s关卡据为所有了", szGBOccupierNick, instName.c_str());
	}
	else if (occupyEmail.type == EINST_OCCUPY_REFRESH_BY_SELF)	//被自己刷新
	{
		sprintf(szContent, "玩家%s重新征服了你一直无法逾越的%s关卡, 并藐视了你一眼", szGBOccupierNick, instName.c_str());
	}
	else if (occupyEmail.type == EINST_OUT_RANK)	//被挤出排行
	{
		sprintf(szContent, "玩家%s把你从%s关卡的排行榜中踢了出去! 像你这点技术，还是回家洗洗睡觉了好", szGBOccupierNick, instName.c_str());
	}
	email.pGBBody = szContent;
	GetEmailCleanMan()->SyncSendEmail(email);
}
