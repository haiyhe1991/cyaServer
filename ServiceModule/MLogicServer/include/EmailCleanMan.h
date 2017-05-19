#ifndef __EMAIL_CLEAN_MAN_H__
#define __EMAIL_CLEAN_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
邮件过期清理
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
	//邮件信息
	struct SSyncEmailSendContent
	{
		UINT32 senderId;	//发送者id
		UINT32 receiverRoleId;	//接收角色id
		UINT32 receiverUserId;	//接收账号id
		BYTE   type;		//邮件类型
		const char* pGBSenderNick;	//发送者昵称
		const char* pGBCaption;	//标题
		const char* pGBBody;		//正文
		UINT64 gold;	//金币
		UINT32 rpcoin;	//代币
		UINT64 exp;		//经验
		UINT32 vit;		//体力 
		BYTE   attachmentsNum;	//附件个数
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
		UINT32 senderId;	//发送者id
		UINT32 receiverRoleId;	//接收角色id
		UINT32 receiverUserId;	//接收账号id
		BYTE   type;		//邮件类型
		std::string strGBSenderNick;	//发送者昵称
		std::string strGBCaption;		//标题
		std::string strGBBody;		//正文
		UINT64 gold;	//金币
		UINT32 rpcoin;	//代币
		UINT64 exp;		//经验
		UINT32 vit;		//体力 
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
		BYTE hasPackProp;	//是否拥有背包物品
		BYTE hasDress;		//是否拥有背包物品
		BYTE hasMagic;		//是否拥有魔导器
		BYTE hasFragment;	//是否拥有装备碎片
		BYTE hasChainSoulFragment;		//是否拥有炼魂碎片
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

	//获取角色未读邮件封数量
	UINT32 GetRoleUnReadEmails(UINT32 roleId);
	//查询邮件列表
	int QueryEmailList(UINT32 roleId, UINT16 from, BYTE num, SQueryEmailListRes* pEmailList);
	//读取邮件内容
	int ReadEmailContent(UINT32 emailId, SQueryEmailContentRes* pEmailContent, int& nBytes);
	//收取邮件附件
	int ReceiveEmailAttachments(UINT32 emailId, BYTE& emialType, SBigNumber& gold, UINT32& rpcoin, SBigNumber& exp, UINT32& vit,
		SEmailAttachmentsProp& hasPropType, SPropertyAttr* pAttachmentsProp, BYTE maxNum, BYTE& retNum);
	//设置邮件读取状态
	int SetEmailReadStatus(UINT32 emailId, BOOL isRead);
	//设置邮件附件提取取状态
	int SetEmailAttachmentsStatus(UINT32 emailId, BOOL isGet);
	//同步发邮件
	int SyncSendEmail(const SSyncEmailSendContent& email);
	//异步发邮件
	void AsyncSendEmail(const SSyncEmailSendContent& email);
	//删除邮件
	int DelEmails(UINT32 roleId, UINT32* pEmailId, BYTE idNum);
	//是否拥有副本掉落邮件
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