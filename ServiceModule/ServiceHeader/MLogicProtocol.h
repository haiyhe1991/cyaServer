#ifndef __MLOGIC_PROTOCOL_H__
#define __MLOGIC_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceMLogic.h"

#pragma pack(push, 1)

//��ѯ��ɫ������Ϣ(linker�������ݰ�)
struct SLinkerQueryRoleAppearanceReq	//linker->MLogicServer
{
	UINT32 userId;		//�˺�id
	UINT32 roleId;		//��ɫId
	UINT32 playerId;	//��ҽ�ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		playerId = htonl(playerId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		playerId = ntohl(playerId);
	}
};

//��ѯ��ɫ��ϸ��Ϣ(linker�������ݰ�)
struct SLinkerQueryRoleExplicitReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫId
	UINT32 playerId;	//��ҽ�ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		playerId = htonl(playerId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		playerId = ntohl(playerId);
	}
};

//�����ɫ����(linker�������ݰ�)
struct SLinkerEnterRoleReq	//linker->MLogicServer
{
	UINT32 userId;	//�˻�id;
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//�˳���ɫ����(linker�������ݰ�)
struct SLinkerLeaveRoleReq	//linker->MLogicServer
{
	UINT32 userId;	//�˻�id;
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ����(linker�������ݰ�)
struct SLinkerQueryBackpackReq	//linker->MLogicServer
{
	UINT32 userId;	//�˻�id
	UINT32 roleId;	//��ɫid
	BYTE from;	//��ʼλ��;
	BYTE num;	//����;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ�ɾ�(linker�������ݰ�)
struct SLinkerQueryAchievementReq	//linker->MLogicServer
{
	UINT32 userId;							//�û�ID
	UINT32 roleId;							//��ɫID
	BYTE type;								//1�ɳ�֮·��2ð�����̣�3��������ID
	UINT16 start_pos;						//��ʼ��ѯλ��

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		start_pos = htons(start_pos);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		start_pos = ntohs(start_pos);
	}
};

//������ɫ(linker�������ݰ�)
struct SLinkerCreateRoleReq	//linker->MLogicServer
{
	UINT32 userId;		//�˺�id
	BYTE occuId;		//ְҵid;
	BYTE sex;			//�Ա�
	char   nick[33];	//�����ǳ�[33];
	void hton()
	{
		userId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
	}
};

//��ѯ�ѽ�������(linker�������ݰ�)
struct SLinkerQueryAcceptTaskReq
{
	UINT32 userId;		//�˺�id
	UINT32 roleId;		//��ɫid;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ����(linker�������ݰ�)
struct SLinkerQueryFriendsReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE from;		//��ʼλ��;
	BYTE num;		//����;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ��������
struct SLinkerQueryFriendApplyReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE from;		//��ʼλ��;
	BYTE num;		//����;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��Ӻ���(linker�������ݰ�)
struct SLinkerAddFriendReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	char nick[33];		//�����ǳ�[33];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��������ȷ��
struct SLinkerFriendApplyConfirmReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 friendUserId;	//�����˺�id
	UINT32 friendRoleId;	//���ѽ�ɫid
	BYTE   status;			//0-�ܾ�, 1-ͬ��
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
		friendUserId = htonl(friendUserId);
		friendRoleId = htonl(friendRoleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		friendUserId = ntohl(friendUserId);
		friendRoleId = ntohl(friendRoleId);
	}
};

//��ȡ�Ƽ������б�
struct SLinkerGetRecommendFriendsReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//�������
struct SLinkerPlayerSearchReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	char playerNick[33];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//ɾ������(linker�������ݰ�)
struct SLinkerDelFriendReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	char nick[33];		//ɾ�������ǳ�[33];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ����ɸ�����Ϣ(linker�������ݰ�)
struct SLinkerQueryFinishedInstReq	//linker->MLogicServer
{
	UINT32 roleId;	//��ɫid
	UINT16 from;	//��ʼλ��;
	BYTE   num; 	//��ѯ����;
	void hton()
	{
		roleId = htonl(roleId);
		from = htons(from);
	}

	void ntoh()
	{
		roleId = ntohl(roleId);
		from = ntohs(from);
	}
};

//�����������(linker�������ݰ�)
struct SLinkerFinishHelpReq		//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 helpId;	//����id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		helpId = htons(helpId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		helpId = ntohs(helpId);
	}
};

//��ѯ���������������Ϣ(linker�������ݰ�)
struct SLinkerQueryFinishedHelpReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//������Ʒ����(linker�������ݰ�)
struct SLinkerSellPropertyReq		//linker->MLogicServer
{
	UINT32 userId;
	UINT32 roleId;	//��ɫid
	UINT32 id;	//��ƷΨһID;
	BYTE   num;	//��Ʒ����;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		id = htonl(id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		id = ntohl(id);
	}
};

//������������(linker�������ݰ�)
struct SLinkerDropPropertyReq		//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE from;      //0-����, 1-�ֿ�
	UINT32 id;      //����ΨһID;
	BYTE num; 	    //������������;
	void hton()
	{
		id = htonl(id);
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		id = ntohl(id);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��������(linker�������ݰ�)
struct SLinkerDropTaskReq		//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 taskId;	//����id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
	}
};

//���½�ɫλ����Ϣ(linker�������ݰ�)
struct SLinkerUpdateRolePosReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	char xyz[16];	//xyz����[16];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//���򱳰���ֿ�洢�ռ�(linker�������ݰ�)
struct SLinkerBuyStorageSpaceReq		//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE buyType;   //0.����  1.�ֿ�
	BYTE num;		//���������		
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��������(linker�������ݰ�)
struct SLinkerAcceptTaskReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 taskId;	//����id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
	}
};

//��ȡ�ʼ�����(linker�������ݰ�)
struct SLinkerReceiveEmailAttachmentsReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 emailId;	//�ʼ�id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		emailId = htonl(emailId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		emailId = ntohl(emailId);
	}
};

//��ѯ�ʼ��б�
struct SLinkerQueryEmailListReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 from;	//��ʼλ��;
	BYTE num;	//����;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		from = htons(from);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		from = ntohs(from);
	}
};

//��ѯ�ʼ�����
struct SLinkerQueryEmailContentReq	//����
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 emailId; //�ʼ�ID
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		emailId = htonl(emailId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		emailId = ntohl(emailId);
	}
};

//ɾ���ʼ�
struct SLinkerDelEmailReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE num;	//ɾ���ʼ�����
	UINT32 emailIds[1];	//ɾ���ʼ�id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		for (BYTE i = 0; i < num; ++i)
			emailIds[i] = htonl(emailIds[i]);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		for (BYTE i = 0; i < num; ++i)
			emailIds[i] = ntohl(emailIds[i]);
	}
};

//����װ��(linker�������ݰ�)
struct SLinkerLoadEquipmentReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 id;	//����ΨһID;
	BYTE   pos;	//λ��;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		id = htonl(id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		id = ntohl(id);
	}
};

//ж��װ��(linker�������ݰ�)
struct SLinkerUnLoadEquipmentReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 id;	//����ΨһID;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		id = htonl(id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		id = ntohl(id);
	}
};

//ʹ�õ���(linker�������ݰ�)
struct SLinkerUsePropertyReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 id;	//����ΨһID;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		id = htonl(id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		id = ntohl(id);
	}
};

//��ȡ�ɾͽ���(linker�������ݰ�)
struct SLinkerReceiveRewardReq	//linker->MLogicServer
{
	UINT32 userId;							//�û�ID
	UINT32 roleId;							//��ɫID
	UINT32 achievement_id;					//�ɾ�ID

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		achievement_id = htonl(achievement_id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		achievement_id = ntohl(achievement_id);
	}
};

//�һ�����(linker�������ݰ�)
struct SLinkerCashPropertyReq		//linker->MLogicServer
{
	UINT32 userId;		//�˺�id
	UINT32 roleId;		//��ɫid
	UINT16 targetId;	//�һ�Ŀ��ID;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		targetId = htons(targetId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		targetId = ntohs(targetId);
	}
};

//ɾ����ɫ(linker�������ݰ�)
struct SLinkerDelRoleReq	//linker->MLogicServer
{
	UINT32 userId;		//�˺�id
	UINT32 roleId;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//�˺��˳�(linker�������ݰ�)
struct SLinkerUserExitReq	//linker->MLogicServer
{
	UINT32 userId;		//�˺�id
	void hton()
	{
		userId = htonl(userId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
	}
};

//�û��ύ��ʱ����
struct SLinkerPersonalTimerTaskReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 taskId;	//��ʱ������
	void hton()
	{
		taskId = htons(taskId);
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		taskId = ntohs(taskId);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ��ɫ����
struct SLinkerQuerySkillsReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ��ɫ����
struct SLinkerQueryCombosReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��������
struct SLinkerSkillUpgradeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 skillId;	//Ҫ�����ļ���id
	BYTE   step;	//��������
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		skillId = htons(skillId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		skillId = ntohs(skillId);
	}
};

//�ı���Ӫ
struct SLinkerChangeFactionReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE factionId;	//��Ӫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//װ�ؼ���
struct SLinkerLoadSkillReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 skillId;	//����id
	BYTE   key;		//��Ӧ�ͻ���key
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		skillId = htons(skillId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		skillId = ntohs(skillId);
	}
};

//ȡ������
struct SLinkerCancelSkillReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 skillId;	//����id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		skillId = htons(skillId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		skillId = ntohs(skillId);
	}
};

//�����ѽ�����Ŀ��
struct SLinkerUpdateTaskTargetReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 taskId;		//����id
	UINT32 target[MAX_TASK_TARGET_NUM];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
		for (BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = htonl(target[i]);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
		for (BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = ntohl(target[i]);
	}
};

//�ύ�������
struct SLinkerFinishTaskReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 taskId;	//����id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
	}
};

//��ȡ������, MLS_RECEIVE_TASK_REWARD
struct SLinkerReceiveTaskRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 taskId;	//����id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
	}
};

//��ѯ�����������ʷ��¼
struct SLinkerQueryFinishedTaskReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 from;	//�Ӷ��ٿ�ʼ
	UINT16 num;		//�������

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		from = htons(from);
		num = htons(num);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		from = ntohs(from);
		num = ntohs(num);
	}
};

//��ȡ�ճ�����
struct SLinkerGetDailyTaskReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//װ���ϳ�
struct SLinkerEquipComposeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 composeId;	//�ϳ�id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		composeId = htons(composeId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		composeId = ntohs(composeId);
	}
};

//װ��λǿ��
struct SLinkerEquipPosStrengthenReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE pos;	//װ��λ
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//װ��λ����
struct SLinkerEquipPosRiseStarReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE pos;	//װ��λ
	SRiseStarAttachParam param;	//���Ǹ��Ӳ���
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//װ��λ��Ƕ��ʯ, MLS_EQUIP_POS_INLAID_GEM
struct SLinkerEquipPosInlaidGemReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE   pos;	//װ��λ;
	UINT32 gemId;		//��ʯΨһID;
	BYTE   holeSeq;		//��ʯ�׺�(0-3)

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		gemId = htonl(gemId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		gemId = ntohl(gemId);
	}
};

//װ��λȡ����ʯ, MLS_EQUIP_POS_REMOVE_GEM
struct SLinkerEquipPosRemoveGemReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE   pos;	//װ��λ;
	BYTE   holeSeq;	//ȡ��Ŀ��װ����ʯ�׺�;  //0-3;

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ȡװ��λ����
struct SLinkerGetEquipPosAttrReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ʯ�ϳ�, MLS_GEM_COMPOSE
struct SLinkerGemComposeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE num;	//��������
	SConsumeProperty material[1];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		for (BYTE i = 0; i < num; ++i)
			material[i].hton();
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		for (BYTE i = 0; i < num; ++i)
			material[i].ntoh();
	}
};

//��Ʒ�ع�
struct SLinkerPropBuyBackReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 sellId;	//����id
	BYTE   num;		//�ع�����
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		sellId = htonl(sellId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		sellId = ntohl(sellId);
	}
};

//��ѯ��ɫ������Ʒ��¼
struct SLinkerQueryRoleSellPropRcdReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 from;	//�ӵڼ�����ʼ
	BYTE   num;		//�������
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		from = htonl(from);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		from = ntohl(from);
	}
};

//��������
struct SLinkerBackpackCleanupReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ֵ
struct SLinkerRechargeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 cashId;	//��ֵid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		cashId = htons(cashId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		cashId = ntohs(cashId);
	}
};

//��ȡ��ɫռ�츱��
struct SLinkerGetRoleOccupyInstReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//���ý�ɫְҵ�ƺ�id
struct SLinkerSetRoleTitleReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 titleId;	//�ƺ�id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		titleId = htons(titleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		titleId = ntohs(titleId);
	}
};

//�̳ǹ�����Ʒ
struct SLinkerMallBuyGoodsReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 buyId;	//��Ʒid
	UINT16 num;		//�������
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		buyId = htonl(buyId);
		num = htons(num);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		buyId = ntohl(buyId);
		num = ntohs(num);
	}
};

//��ѯ��ɫʱװ
struct SLinkerQueryRoleDressReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//����ʱװ
struct SLinkerWearDressReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 dressId;		//ʱװid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		dressId = htons(dressId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		dressId = ntohs(dressId);
	}
};

//���븱��(linker�������ݰ�)
struct SLinkerEnterInstReq	//linker->MLogicServer
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 instId;	// ����ID;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//��ȡ��������ֵ���
struct SLinkerFetchInstAreaDropsReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 instId;	//����ID;
	BYTE   areaId;	//����id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//ʰȡ�����ֵ���
struct SLinkerPickupInstMonsterDropReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 instId;	//����id
	BYTE   areaId;	//����id
	BYTE   dropId;	//������(SMonsterDropPropItem:id)
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//ʰȡ��������
struct SLinkerPickupInstBoxReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 instId;	//����id
	BYTE   boxId;	//����id
	BYTE   id;		//��������Ʒ���
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//ʰȡ���������
struct SLinkerPickupBrokenDropReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 instId;	//����id
	BYTE   dropId;	//������(SMonsterDropPropItem:id)
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//ɱ�ֵþ���
struct SLinkerKillMonsterExpReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 instId;		//����id
	BYTE   areaId;		//����id
	UINT16 monsterId;	//��id
	BYTE   monsterLv;	//�ֵȼ�

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
		monsterId = htons(monsterId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
		monsterId = ntohs(monsterId);
	}
};

//���ĳ����(linker�������ݰ�)
struct SLinkerFinishInstReq	//linker->MLogicServer
{
	UINT32 userId;			//�˺�id
	UINT32 roleId;			//��ɫid
	UINT16 instId;			//����ID;
	UINT32 score;			//����
	BYTE   star;			//����
	UINT16 finCode;			//�����(0-�ɹ�������-ʧ��)

	/* zpy �ɾ�ϵͳ���� */
	UINT32 continuous;		//��������
	bool speed_evaluation;	//�ٶ�����
	bool hurt_evaluation;	//�˺�����

	/* zpy ��ʱ����� */
	UINT32 surplus_time;	//ʣ��ʱ��
	UINT32 resist_number;	//�ֵ�����
	UINT32 kill_number;		//��ɱ��������
	INT32 level_gap;		//�ȼ���
	INT32 exp_gap;			//�����
	INT32 cp_gap;			//ս����
	UINT32 hurt_sum;		//�˺�����
	BYTE chest_num;			//��������
	SChestItem data[1];		//������Ϣ

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
		score = htonl(score);
		finCode = htons(finCode);

		/* zpy �ɾ�ϵͳ���� */
		continuous = htonl(continuous);

		/* zpy ��ʱ����� */
		surplus_time = htonl(surplus_time);
		resist_number = htonl(resist_number);
		kill_number = htonl(kill_number);
		level_gap = htonl(level_gap);
		exp_gap = htonl(exp_gap);
		cp_gap = htonl(cp_gap);
		hurt_sum = htonl(hurt_sum);
		for (BYTE i = 0; i < chest_num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
		score = ntohl(score);
		finCode = ntohs(finCode);

		/* zpy �ɾ�ϵͳ���� */
		continuous = ntohl(continuous);

		/* zpy ��ʱ����� */
		surplus_time = ntohl(surplus_time);
		resist_number = ntohl(resist_number);
		kill_number = ntohl(kill_number);
		level_gap = ntohl(level_gap);
		exp_gap = ntohl(exp_gap);
		cp_gap = ntohl(cp_gap);
		hurt_sum = ntohl(hurt_sum);
		for (BYTE i = 0; i < chest_num; ++i)
			data[i].ntoh();
	}
};

//��ȡ����������
struct SLinkerFetchRobotReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE rule;		//ƥ�����
	UINT32 targetId;//Ŀ���ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		targetId = htonl(targetId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		targetId = ntohl(targetId);
	}
};

//��ȡ������������������������
struct SLinkerGetPersonalTransferEnterReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 transferId;	//������id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		transferId = htons(transferId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		transferId = ntohs(transferId);
	}
};

//��ѯװ����Ƭ
struct SLinkerQueryEquipFragmentReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ɫ����
struct SLinkerReviveRoleReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 reviveId;	//����id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		reviveId = htons(reviveId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		reviveId = ntohs(reviveId);
	}
};

//��ȡ�Ѵ򿪵ĵ����������
struct SLinkerGetOpenPersonalRandInstReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ��Ҷһ���Ϣ
struct SLinkerQueryGoldExchangeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��Ҷһ�
struct SLinkerGoldExchangeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯδ��ȡ������VIP�ȼ�
struct SLinkerQueryNotReceiveVIPRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ȡVIP����, MLS_RECEIVE_VIP_REWARD
struct SLinkerReceiveVIPRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE   vipLv;	//vip�ȼ�
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ�ѳ�ֵ�����׳����͵ĳ�ֵid
struct SLinkerQueryRechargeFirstGiveReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ��ɫӵ��ħ����
struct SLinkerQueryRoleMagicsReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//���ħ����
struct SLinkerGainMagicReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 instId;	//����id
	UINT16 magicId;	//ħ����id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
		magicId = htons(magicId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
		magicId = ntohs(magicId);
	}
};

//����ɨ��
struct SLinkerInstSweepReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 instId;	//����id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//��ѯ��ǰǩ��
struct SLinkerQueryOnlineSignInReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//����ǩ��
struct SLinkerOnlineSignInReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ�Ѿ���ȡ��б�
struct SLinkerQueryReceivedActivityReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ȡ�����
struct SLinkerReceiveActivityReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 activityId;	//�id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		activityId = htonl(activityId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		activityId = ntohl(activityId);
	}
};

//��ѯ��ǰ�����½�
struct SLinkerQueryCurOpenChapterReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ȡ�һ��뽱��
struct SLinkerReceiveCashCodeRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	char cashCode[33];	//�һ���
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ��������
struct SLinkerQueryUnlockContentReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE type;		//��������

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//���ݽ���, MLS_CONTENT_UNLOCK
struct SLinkerContentUnlockReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE type;		//��������
	UINT16 unlockId;	//����id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		unlockId = htons(unlockId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		unlockId = ntohs(unlockId);
	}
};

//��ȡ���̽�Ʒ
struct SLinkerGetRoulettePropReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ȡ���̽�Ʒ
struct SLinkerExtractRouletteRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ȡ��ֵ����
struct SLinkerFetchRechargeOrderReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 partId;	//����id
	UINT16 rechargeId;	//��ֵid
	UINT32 rmb;			//�����
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		partId = htons(partId);
		rechargeId = htons(rechargeId);
		rmb = htonl(rmb);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		partId = ntohs(partId);
		rechargeId = ntohs(rechargeId);
		rmb = ntohl(rmb);
	}
};

//��ȡ��ֵ���ش���
struct SLinkerGetRechargeTokenReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	char   orderNO[65]; //������

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//ˢ�����̴�
struct SLinkerUpdateRouletteBigRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ȡ���̴�
struct SLinkerFetchRouletteBigRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ȡ�����������ս����
struct SLinkerGetActiveInstChallengeTimesReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

/* zpy �����һ� */
//��ѯ�����һ���Ϣ
struct SLinkerQueryVitExchangeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//�����һ�
struct SLinkerVitExchangeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

/* zpy ��ѯ��ɫÿ��ǩ�� */
struct SLinkerQueryRoleDailySignReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

/* zpy ÿ��ǩ�� */
struct SLinkerRoleDailySignReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ������Ƭ
struct SLinkerQueryChainSoulFragmentReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ���겿��
struct SLinkerQueryChainSoulPosReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//���겿������
struct SLinkerChainSoulPosUpgradeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE pos;		//��������

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//add by hxw 20151028
//��ѯ�ȼ����а���Ϣ
struct SLinkerQueryLvInfoReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};
//end

/* by hxw 20151006 */
//��ѯ�ȼ����еĽ�����Ϣ �������� �� �Ƿ��ȡ����
struct SLinkerQueryLvRankRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

//��ѯ������ս�������еĽ�����Ϣ �������� �� �Ƿ��ȡ����
struct SLinkerQueryInsnumRankRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

//��ѯ������ս����������Ϣ ���� ���а��е���Ϣ
struct SLinkerQueryInsnumRankInfoReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

/* by hxw 20151006 ��ȡLV���н���*/
struct SLinkerGetLvRankRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

//��ȡ������ս���н�������
struct SLinkerGetInsnumRankRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

// add by hxw 20151015 ǿ�߽������
//��ȡ��ȡ����ǿ�߽�������
struct SLinkerQueryStrongerInfoReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};
//��ȡǿ�߽���
struct SLinkerGetStrongerReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 rewardId; //����ID
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		rewardId = htons(rewardId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		rewardId = ntohs(rewardId);
	}
};
//end at 20151015

//add by hxw 20151020 ��ȡ���߽�����Ϣ
struct SLinkerQueryOnlineInfoReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};
// ��ȡ���߽���
struct SLinkerGetOnlineRewardReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 rewardId; //����ID
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		rewardId = htons(rewardId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		rewardId = ntohs(rewardId);
	}
};
//end at 20151020

//��������Boos
struct SLinkerAttackWorldBossReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 hurt;	//�˺�����

	void hton()
	{
		hurt = htonl(hurt);
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		hurt = ntohl(hurt);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//��ѯ����Boss���а�
struct SLinkerQueryWorldBossRankReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ������Bossɱ��
struct SLinkerWasKilledByWorldBossReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ������Boss�����и���
struct SLinkerResurrectionInWorldBossReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ��ѯ����Boss��Ϣ
struct SLinkerQueryWorldBossInfoReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ��ѯ������Ϣ
struct SLinkerLadderRoleInfoReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ����ƥ�����
struct SLinkerMatchingLadderReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ˢ������������Ϣ
struct SLinkerRefreshLadderRankReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ������ս���
struct SLinkerChallengeLadderReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT32 targetId;					//Ŀ��ID
	UINT32 target_rank;					//Ŀ������

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		targetId = htonl(targetId);
		target_rank = htonl(target_rank);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		targetId = ntohl(targetId);
		target_rank = ntohl(target_rank);
	}
};

// ��������CD
struct SLinkerEliminateLadderCDTimeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ���������ս
struct SLinkerFinishLadderChallengeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE finishCode;					//�����(0-�ɹ�������-ʧ��)

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ��ѯ���н�ɫ
struct SLinkerQueryAllRolesReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ��ѯӵ�еĻ��������
struct SLinkerHasActivityInstTypeReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ��ѯ�����ɫ
struct SLinkerQueryQueryWeekFreeActorReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ��ѯ����ģʽ�������
struct SLinkerQueryCompEnterNumReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ���򾺼�ģʽ�������
struct SLinkerBuyCompEnterNumReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	BYTE buy_mode;

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ��ѯ1v1ģʽ�������
struct SLinkerQuery1V1EnterNumReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// ����1v1ģʽ�������
struct SLinkerBuy1V1EnterNumReq
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 buy_num;

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		buy_num = htons(buy_num);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		buy_num = ntohs(buy_num);
	}
};

#pragma pack(pop)

#endif