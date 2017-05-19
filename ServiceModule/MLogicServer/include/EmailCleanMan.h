#ifndef __EMAIL_CLEAN_MAN_H__
#define __EMAIL_CLEAN_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�ʼ���������
*/

#include <list>
#include <vector>
#include "cyaThread.h"
#include "cyaSync.h"
#include "cyaLocker.h"
#include "ServiceMLogic.h"

class EmailCleanMan
{
	EmailCleanMan(const EmailCleanMan&);
	EmailCleanMan& operator = (const EmailCleanMan&);

public:
	EmailCleanMan();
	~EmailCleanMan();

#pragma pack(push, 1)
	//�ʼ���Ϣ
	struct SSyncEmailSendContent
	{
		UINT32 senderId;	//������id
		UINT32 receiverRoleId;	//���ս�ɫid
		UINT32 receiverUserId;	//�����˺�id
		BYTE   type;		//�ʼ�����
		const char* pGBSenderNick;	//�������ǳ�
		const char* pGBCaption;	//����
		const char* pGBBody;		//����
		UINT64 gold;	//���
		UINT32 rpcoin;	//����
		UINT64 exp;		//����
		UINT32 vit;		//���� 
		BYTE   attachmentsNum;	//��������
		SPropertyAttr* pAttachments;
		SSyncEmailSendContent()
		{
			senderId = 0;
			receiverRoleId = 0;
			receiverUserId = 0;
			type = 0;
			pGBSenderNick = NULL;
			pGBCaption = NULL;
			pGBBody = NULL;
			gold = 0;
			rpcoin = 0;
			attachmentsNum = 0;
			exp = 0;
			vit = 0;
			pAttachments = NULL;
		}
	};
	struct SAsyncEmailSendContent
	{
		UINT32 senderId;	//������id
		UINT32 receiverRoleId;	//���ս�ɫid
		UINT32 receiverUserId;	//�����˺�id
		BYTE   type;		//�ʼ�����
		std::string strGBSenderNick;	//�������ǳ�
		std::string strGBCaption;		//����
		std::string strGBBody;		//����
		UINT64 gold;	//���
		UINT32 rpcoin;	//����
		UINT64 exp;		//����
		UINT32 vit;		//���� 
		std::vector<SPropertyAttr> attachmentsVec;
		SAsyncEmailSendContent()
		{
			senderId = 0;
			receiverRoleId = 0;
			receiverUserId = 0;
			type = 0;
			strGBSenderNick = "";
			strGBCaption = "";
			strGBBody = "";
			gold = 0;
			rpcoin = 0;
			exp = 0;
			vit = 0;
			attachmentsVec.clear();
		}
	};

	struct SEmailAttachmentsProp
	{
		BYTE hasPackProp;	//�Ƿ�ӵ�б�����Ʒ
		BYTE hasDress;		//�Ƿ�ӵ�б�����Ʒ
		BYTE hasMagic;		//�Ƿ�ӵ��ħ����
		BYTE hasFragment;	//�Ƿ�ӵ��װ����Ƭ
		BYTE hasChainSoulFragment;		//�Ƿ�ӵ��������Ƭ
		SEmailAttachmentsProp()
		{
			hasPackProp = 0;
			hasDress = 0;
			hasMagic = 0;
			hasFragment = 0;
			hasChainSoulFragment = 0;
		}
	};

#pragma pack(pop)

public:
	void Start();
	void Stop();

	//��ȡ��ɫδ���ʼ�������
	UINT32 GetRoleUnReadEmails(UINT32 roleId);
	//��ѯ�ʼ��б�
	int QueryEmailList(UINT32 roleId, UINT16 from, BYTE num, SQueryEmailListRes* pEmailList);
	//��ȡ�ʼ�����
	int ReadEmailContent(UINT32 emailId, SQueryEmailContentRes* pEmailContent, int& nBytes);
	//��ȡ�ʼ�����
	int ReceiveEmailAttachments(UINT32 emailId, BYTE& emialType, SBigNumber& gold, UINT32& rpcoin, SBigNumber& exp, UINT32& vit,
		SEmailAttachmentsProp& hasPropType, SPropertyAttr* pAttachmentsProp, BYTE maxNum, BYTE& retNum);
	//�����ʼ���ȡ״̬
	int SetEmailReadStatus(UINT32 emailId, BOOL isRead);
	//�����ʼ�������ȡȡ״̬
	int SetEmailAttachmentsStatus(UINT32 emailId, BOOL isGet);
	//ͬ�����ʼ�
	int SyncSendEmail(const SSyncEmailSendContent& email);
	//�첽���ʼ�
	void AsyncSendEmail(const SSyncEmailSendContent& email);
	//ɾ���ʼ�
	int DelEmails(UINT32 roleId, UINT32* pEmailId, BYTE idNum);
	//�Ƿ�ӵ�и��������ʼ�
	BOOL IsHasInstDropEmail(UINT32 roleId);

private:
	static int THWorker(void* param);
	int OnWorker();

	void OnSendEmail();
	void OnCleanReadEmails(LTMSEL ltNow);
	void OnCleanUnReadEmails(LTMSEL ltNow);
	void NewEmailNotify(UINT32 userId, UINT32 roleId);

private:
	OSThread m_thHandle;
	BOOL m_threadExit;
	InterruptableSleep m_sleep;
	LTMSEL m_lastCleanMSel;

	CXTLocker m_asyncEmailListLocker;
	std::list<SAsyncEmailSendContent> m_asyncEmailList;
};

void InitEmailCleanMan();
EmailCleanMan* GetEmailCleanMan();
void DestroyEmailCleanMan();

#endif