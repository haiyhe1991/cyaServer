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
				//ռ����
				gwsSession->SendResponse(curUserLinkerId, curUserId, MLS_OK, MLS_INST_OCCUPY_PUSH_NOTIFY, buf, nLen);
				//��ռ����
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

	char szGBOccupierNick[MAX_UTF8_NICK_LEN] = { 0 };	//ռ�����ǳ�
	utf8_gb2312(occupyEmail.occupierNick.c_str(), (int)occupyEmail.occupierNick.length(), szGBOccupierNick, sizeof(szGBOccupierNick)-sizeof(char));

	char szContent[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
	EmailCleanMan::SSyncEmailSendContent email;
	email.pGBSenderNick = "ϵͳ";
	email.pGBCaption = "����ռ��";
	email.receiverRoleId = occupyEmail.occupiedRoleId;
	email.receiverUserId = occupyEmail.occupiedUserId;

	if (occupyEmail.type == EINST_OCCUPY_BY_NEW_ROLE)		//���µ����ռ��
	{
		sprintf(szContent, "���%s��ȫ������Ĵ���, �����ɵľͰ����%s�ؿ���Ϊ������", szGBOccupierNick, instName.c_str());
	}
	else if (occupyEmail.type == EINST_OCCUPY_REFRESH_BY_SELF)	//���Լ�ˢ��
	{
		sprintf(szContent, "���%s������������һֱ�޷���Խ��%s�ؿ�, ����������һ��", szGBOccupierNick, instName.c_str());
	}
	else if (occupyEmail.type == EINST_OUT_RANK)	//����������
	{
		sprintf(szContent, "���%s�����%s�ؿ������а������˳�ȥ! ������㼼�������ǻؼ�ϴϴ˯���˺�", szGBOccupierNick, instName.c_str());
	}
	email.pGBBody = szContent;
	GetEmailCleanMan()->SyncSendEmail(email);
}
