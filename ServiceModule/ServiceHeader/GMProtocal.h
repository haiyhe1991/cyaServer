#ifndef __GM_PROTOCAL_H__
#define __GM_PROTOCAL_H__

#include "cyaBase.h"
#include "ServiceCommon.h"

#define DEFAULT_FILE_SIZE	4*1024	//Ĭ���ļ���СΪ4M
#define DEFAULT_MAX_COUNT	288		//ÿ����󱣴�������������
#define IS_RUN_STORE		0		//�Ƿ���д������ģʽ
#define IS_SCREEN_LOG		1		//�Ƿ�����ʾ��־����Ļģʽ
#define IS_RUN_STORE_LK		1		//�Ƿ����������ݴ���dbģʽ

#define LK_ACCOUNT		"account_leavekeep"		//�˺������
#define LK_DEVICE		"device_leavekeep"		//�豸�����
#define LK_ACTOR		"actor_leavekeep"		//��ɫ�����
#define TB_ACCOUNT		"account"				//�˺ű�
#define TB_ACTOR		"actor"					//��ɫ��
#define TB_CASH			"cash"					//��ֵ��

#define ACCOUNT_INFO		"regist_account"		//ע���˺���Ϣ��
#define DEVICE_INFO			"active_device"			//ע���豸��ֻ����ע���豸��������
#define ACTOR_INFO			"creat_actor"			//������ɫ��Ϣ��
#define ACCOUNT_LOGIN_INFO	"account_info"			//�˺���Ϣ����¼ע��͵�½���˺���Ϣ��

#define DB_SGS				"sgs"				//sgs db
#define DB_SGSINFO			"sgsinfo"			//sgsinfo db
#define DB_SGSVERIFY		"sgsverify"			//sgsverify db
#define DB_SGSRECHARGE		"sgsrecharge"		//sgsrecharge db
#define DB_SGS_ACTION		"sgs_action"		//sgs_action db

#define IS_TEST 0	//����

///��������
enum LeaveKeepType
{
	ACCOUNT_LK = 1, //�ʺ�����
	DEVICE_LK = 2, //�豸����
	ACTOR_LK = 3  //��ɫ����
};

///����ʱ������
enum LKTimeType
{
	MORROW_LK = 1, //��������
	THIRD_LK = 3, //��������
	SEVENTH_LK = 7, //��������
	THIRTYTH_LK = 30, //����ʮ������
	MONTH_LK = 90	 //������
};

///ƽ����������
enum AverageOlPerType
{
	OL_PER_SECOND,  //ÿ������
	OL_PER_MINUTE,  //ÿ������
	OL_PER_HOUR,	  //ÿСʱ����
	OL_PER_DAY,    //ÿ������
	OL_PER_MONTH,  //ÿ������
	OL_PER_YEAR    //ÿ������
};

///������������
enum OnlineType
{
	OL_COMM = 1,     //ÿ�ն�ʱˢ����������
	OL_MAX = 2,     //ÿ�������������
	OL_AVERAGE = 3		//ÿ��ƽ����������
};

///��ֵ����
///�׳��û�����Ҫ��������
enum CashType
{
	PY_CASH_ACTOR = 1, //���ո��ѽ�ɫ��
	PY_CASH_COUNT = 2, //���ո����ܴ���
	PY_CASH_TOTAL = 3, //�ۼ����������û���
	PY_CASH_NRMB = 4, //�������������û��ĸ��ѽ��
	PY_CASH_CRMB = 5, //�����ۼƸ��ѽ��
	PY_CASH_TORMB = 6, //�ۼƸ����û���������
};

enum FuncOperateType
{
	ACCOUNT_CREATE = 1, //�˺Ŵ�����
	ADD_NEW_USER = 2,   //�����û���
	LOGIN_USER = 3      //��¼�û���
};

#pragma pack(push, 1)
///GMServer ͨ��Э��ͷ
struct GMProtocalHead
{
	UINT16 pduLen : 12;	 //���ݳ���
	UINT16 pktType : 3;	 //������
	UINT16 isCrypt : 1;	 //�Ƿ����
	UINT32 req;		 //�����к�
	BYTE isOver;      //�Ƿ�Ϊ������

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*((UINT16*)(pThis + offset)) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*((UINT32*)(pThis + offset)) = htonl(*(UINT32*)(pThis + offset));
		offset += sizeof(UINT32);
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*((UINT16*)(pThis + offset)) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*((UINT32*)(pThis + offset)) = ntohl(*(UINT32*)(pThis + offset));
		offset += sizeof(UINT32);
	}
};

///���Ӻ��͵�key���������ʱ��
struct GMConntedTokenHeart
{
	UINT16 heatbeatInterval;  //����ʱ����
	char key[33];				//��Կ

	void hton()
	{
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)pThis = htons(*(UINT16*)pThis);
	}

	void ntoh()
	{
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)pThis = ntohs(*(UINT16*)pThis);
	}
};

///����Ա��¼ GM_USER_LOGIN
struct GMUserLoginReq
{
	char name[33];    //�û���
	char passwd[33];  //����
};

///�����豸��Ϣ
struct ActiveDevInfo
{
	char devIp[16];		//�豸IP
	char tts[20];			//����ʱ��
	char devType[33];		//����
	char sys[33];			//ϵͳ
	char devIdfa[41];		//�豸��IDFA
	char region[41];		//����
};

///�豸���� GM_DEVICE_ACTIVE
struct GMQActiveDeviceReq
{
	BYTE from;			//��ʼ
	BYTE to;				//��ѯ��¼��
	char startTime[20];	//��ʼʱ��
	char endTime[20];		//��ʼʱ��
};

struct GMQActiveDeviceRes
{
	BYTE retNum;			 //��ǰ��������
	UINT32 total;			 //����
	ActiveDevInfo data[1]; //�豸��Ϣ

	void hton()
	{
		int offset = 0;
		offset += sizeof(char);
		BYTE* pThis = (BYTE*)this;
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		offset += sizeof(char);
		BYTE* pThis = (BYTE*)this;
		*(UINT32*)(pThis + offset) = ntohl(*(UINT32*)(pThis + offset));
	}
};

///�����豸 GM_ACCOUNT_CREATE
struct GMQAddNewDeviceReq
{
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	char startTime[20];	//��ѯ��ʼʱ��
	char endTime[20];		//��ѯ��ʼʱ��
	BYTE stype;			//(1-��������(����δ����ɫ)��2-��������������, 3-��ֹ���մ����ܵ��ʺ���)

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMQAddNewDeviceRes
{
	UINT32 count; //�����豸��
	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

///�ʺŴ����� GM_ACCOUNT_CREATE
struct GMQCreateAccountReq
{
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	char startTime[20];	//��ѯ��ʼʱ��
	char endTime[20];		//��ѯ��ֹʱ��
	BYTE stype;			//��������(1-��������(����δ����ɫ)��2-��������������, 3-��ֹ���մ����ܵ��ʺ���)

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMQCreateAccountRes
{
	UINT32 count; //�ʺŴ�����
	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

///��¼�� GM_LOGIN_ACCOUNT
struct GMLoginQueryReq
{
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	char startTime[20];	//��ѯ��ʼʱ��
	char endTime[20];		//��ѯ��ֹʱ��
	BYTE stype;			//��¼����(1-��¼�û�, 2-��¼�豸, 3-��¼��ɫ)

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMLoginQueryRes
{
	UINT32 count; //��¼����
	BYTE stype;   //��¼����(1-��¼�û�, 2-��¼�豸, 3-��¼��ɫ)
	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

/// ������ GM_ADD_NEW_ACCOUNT
struct GMQAddNewAccountReq
{
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	char startTime[20];	//��ѯ��ʼʱ��
	char endTime[20];		//��ѯ��ֹʱ��
	BYTE stype;			//��������(1-�����û�, 2-�����豸, 3-������ɫ)

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMQAddNewAccountRes
{
	UINT32 count; //�ʺŴ�����
	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

/// ��Ծ�� GM_ACTIVE_ACCOUNT
struct GMQActiveUserReq
{
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	char startTime[20];	//��ѯ��ʼʱ��
	char endTime[20];		//��ѯ��ֹʱ��
	BYTE stype;			//����(1-��Ծ�û���, 2-��Ծ�豸��,3-��Ծ��ɫ)

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMQActiveUserRes
{
	UINT32 count; //�ʺŴ�����
	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

/// �ۼ����� GM_ADDUP_NEW
struct GMQAddupNewReq
{
	UINT16 platId;		//ƽ̨ID
	UINT16 channelId;		//����ID
	UINT16 partId;		//����ID

	char startTime[20];	//��ѯ��ʼʱ��
	char endTime[20];		//��ѯ��ֹʱ��
	BYTE stype;			//�ۼ�����(1-�ۼ��û�, 2-�ۼ��豸, 3-�ۼƽ�ɫ)

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMQAddupNewRes
{
	UINT32 count; //�ۼ�����
	BYTE stype;  //����1:�û�,2,�豸,3:��ɫ

	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

///������Ϣ������������
struct OnlineInfo
{
	UINT32 onlineCount; //��������
	char olTime[20];    //��ǰ����ʱ��

	void hton()
	{
		BYTE* pThis = (BYTE*)this;
		*(UINT32*)pThis = htonl(*(UINT32*)pThis);
	}

	void ntoh()
	{
		BYTE* pThis = (BYTE*)this;
		*(UINT32*)pThis = ntohl(*(UINT32*)pThis);
	}
};

///ƽ����Ϸʱ��
struct GMQAverageTimeReq
{
	UINT16 platId;		//ƽ̨ID
	UINT16 channelId;		//����ID
	UINT16 partId;		//����ID

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}

};

///ƽ������ʱ��
struct GMQAverageTimeRes
{
	UINT32 averageTime;

	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

///ƽ����Ϸ����
struct GMQAveragePlayGameReq
{
	UINT16 platId;		//ƽ̨ID
	UINT16 channelId;		//����ID
	UINT16 partId;		//����ID

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

///ƽ����Ϸ����
struct GMQAveragePlayGameRes
{
	UINT16 averagePlayCount; //ƽ����Ϸ����

	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT16*)tThis) = htons(*(UINT16*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT16*)tThis) = ntohs(*(UINT16*)tThis);
	}
};

///����������� GM_MAX_ONLINE_USER_COUNT
struct GMMaxOnlineReq
{

	BYTE to;				    //��ѯ����
	UINT16 platId;				//����ID
	UINT16 partId;				//����ID
	UINT32 from;			    //��ʼֵ
	char startTime[20];			//��ʼʱ��
	char endTime[20];			//����ʱ��

	void hton()
	{
		int offset = 0;
		offset += sizeof(char);
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		offset += sizeof(char);
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}
};

struct GMMaxOnlineReRes
{
	BYTE retNum;			//��������
	UINT32 total;			//���� 
	OnlineInfo info[1];	//������Ϣ 

	void hton()
	{

		for (BYTE i = 0; i < retNum; i++)
			info[i].hton();
		total = htonl(total);
	}

	void ntoh()
	{
		total = ntohl(total);
		for (BYTE i = 0; i < retNum; i++)
			info[i].ntoh();
	}
};

///ƽ���������� GM_AVERAGE_ONLINE_USER_COUNT
struct GMAverageOnlineReq
{
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	char startTime[20];  //��ʼʱ��
	char endTime[20];    //����ʱ��

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMAverageOnlineRes
{
	UINT32 onlineNum; //ƽ����������
	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT32*)(pThis + offset) = ntohl(*(UINT32*)(pThis + offset));
	}

};

///�����û��� GM_PAYMENT_USER_COUNT
struct GMPaymentUserReq
{
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	char startTime[20];  //��ʼʱ��
	char endTime[20];    //����ʱ��

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMPaymentUserRes
{
	UINT32 paymentCount; //�����û���

	void hton()
	{
		paymentCount = htonl(paymentCount);
	}

	void ntoh()
	{
		paymentCount = ntohl(paymentCount);
	}
};

///���Ѵ��� GM_PAYMENT_COUNT
struct GMPaymentCountReq
{
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	char startTime[20];  //��ʼʱ��
	char endTime[20];    //����ʱ��

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMPaymentCountRes
{
	UINT32 paymentCount; //���Ѵ���

	void hton()
	{
		paymentCount = htonl(paymentCount);
	}

	void ntoh()
	{
		paymentCount = ntohl(paymentCount);
	}
};

///���������û��� GM_PAYMENT_USER_COUNT
struct GMFisrstRechargeReq
{
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	char startTime[20];  //��ʼʱ��
	char endTime[20];    //����ʱ��

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMFisrstRechargeRes
{
	UINT32 firstPayCount; //�׳����

	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

///�ۼ����������û��� GM_ADDUP_PAYMENT_COUNT
struct GMAddupPaymentReq
{
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	char startTime[20];  //��ʼʱ��
	char endTime[20];    //����ʱ��

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMAddupPaymentRes
{
	UINT32 addupPayment; //�ۼ����������û���

	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};

///�������ѽ�� GM_ADD_NEW_PAYMENT_MONEY
struct GMAddNewPaymentMoneyReq
{
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	char startTime[20];  //��ʼʱ��
	char endTime[20];    //����ʱ��

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMAddNewPaymentMoneyRes
{
	UINT32 addNewPaymentMoney; //�������ѽ��

	void hton()
	{
		addNewPaymentMoney = htonl(addNewPaymentMoney);
	}

	void ntoh()
	{
		addNewPaymentMoney = ntohl(addNewPaymentMoney);
	}
};

///���ѽ�� GM_PAYMENT_MONEY
struct GMPaymentMoneyReq
{
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	char startTime[20];  //��ʼʱ��
	char endTime[20];    //����ʱ��

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMPaymentMoneyRes
{
	UINT32 paymentMoney; //���ѽ��

	void hton()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = htonl(*(UINT32*)tThis);
	}

	void ntoh()
	{
		BYTE* tThis = (BYTE*)this;
		*((UINT32*)tThis) = ntohl(*(UINT32*)tThis);
	}
};


///�ۼƸ��ѽ�� GM_ADDUP_PAYMENT_MONEY
struct GMPaymentTotalMoneyReq
{
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	char startTime[20];  //��ʼʱ��
	char endTime[20];    //����ʱ��

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMPaymentTotalMoneyRes
{
	UINT32 paymentMoney; //�ۼƸ��ѽ��

	void hton()
	{
		paymentMoney = htonl(paymentMoney);
	}

	void ntoh()
	{
		paymentMoney = ntohl(paymentMoney);
	}
};


///������Ϣ
struct CostInfo
{
	BYTE type;			//��������
	UINT32 actorId;		//��ɫID
	UINT32 money;			//���ѽ��
	UINT32 remain;		//���
	char time[20];		//����ʱ��
	char nick[33];		//�ǳ�
	char action[128];		//������Ϊ

	void hton()
	{
		actorId = htonl(actorId);
		money = htonl(money);
		remain = htonl(remain);
	}

	void ntoh()
	{
		actorId = ntohl(actorId);
		money = ntohl(money);
		remain = ntohl(remain);
	}
};


///�����ܽ����
struct GMQCostTotalReq
{
	BYTE to;			//��ѯ����
	BYTE costType;		//��������(0-��ң� 1-����)
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	UINT32 from;		//��ʼֵ
	char startTime[20];	//��ʼʱ��
	char endTime[20];		//����ʱ��

	void hton()
	{
		int offset = 0;
		offset += sizeof(UINT16);
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		offset += sizeof(UINT16);
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}
};

struct GMQCostTotalRes
{
	BYTE retNum;			//��������
	UINT32 total;			//����
	UINT32 costMoney;		//�����ܽ��
	CostInfo data[1];		//��������


	void hton()
	{
		for (int i = 0; i < retNum; i++)
			data[i].hton();
		total = htonl(total);
		costMoney = htonl(costMoney);
	}

	void ntoh()
	{
		total = htonl(total);
		costMoney = htonl(costMoney);
		for (int i = 0; i < retNum; i++)
			data[i].ntoh();
	}
};

///������ɫ������
struct GMSingleActorCostReq
{
	BYTE costType;		//��������(0-��ң� 1-����)
	BYTE to;				//��ѯ����
	UINT16 platId;		//����ID
	UINT16 partId;		//����ID
	UINT32 from;			//��ѯ��ʼλ��
	char nick[33];		//��ɫ�ǳ�
	char startTime[20];	//��ʼʱ��
	char endTime[20];		//����ʱ��

	void hton()
	{
		int offset = 0;
		offset += sizeof(UINT16);
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		offset += sizeof(UINT16);
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT32*)(pThis + offset) = ntohl(*(UINT32*)(pThis + offset));
	}
};


struct  GMSingleActorCostRes
{
	BYTE retNum;			//��������
	UINT32 total;			//����
	UINT32 costMoney;		//�����ܽ��
	CostInfo data[1];		//��������


	void hton()
	{
		for (BYTE i = 0; i < retNum; i++)
			data[i].hton();
		total = htonl(total);
		costMoney = htonl(costMoney);
	}

	void ntoh()
	{
		total = htonl(total);
		costMoney = htonl(costMoney);
		for (int i = 0; i < retNum; i++)
			data[i].ntoh();
	}
};


///�ʺ���Ϣ
struct AccountInfo
{
	BYTE status; //״̬
	UINT16 partId; //����ID
	UINT32 id; //�ʺ�ID
	UINT32 visit; //��¼����
	char ip[16]; //ע��IP
	char platsource[17]; //ע������
	char os[17]; //�û�ע��ʹ�ò���ϵͳ
	char model[17]; //����
	char ver[17]; //��ǰ���汾
	char regtts[20]; //ע��ʱ��
	char lasttts[20]; //���һ�ε�¼ʱ��
	char name[33]; //�û���
	char passwd[33]; //����
	char regdev[65]; //ע���豸��
	char lastdev[65]; //���һ�ε�¼�豸��

	void hton()
	{
		int offset = 0;
		offset += sizeof(char);
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
		offset += sizeof(UINT32);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		offset += sizeof(char);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT32*)(pThis + offset) = ntohl(*(UINT32*)(pThis + offset));
		offset += sizeof(UINT32);
		*(UINT32*)(pThis + offset) = ntohl(*(UINT32*)(pThis + offset));
	}

};

///��ѯ�ʺ�����
struct GMQAccountDetailInofReq
{

	BYTE to; //��ѯ���� 
	BYTE type; //����(1-����, 2-��¼)
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	UINT32 from; //��ʼλ
	char startTime[20]; //��ʼʱ��
	char endTime[20]; //��ֹʱ��

	void hton()
	{
		platId = htons(platId);
		partId = htons(partId);
		from = htonl(from);
	}

	void ntoh()
	{
		platId = ntohs(platId);
		partId = ntohs(partId);
		from = ntohl(from);
	}

};



struct GMQAccountDetailInofRes
{
	BYTE type; //��ѯ����(1-ע�ᣬ2-��¼)
	BYTE retNum; //��ǰ������
	UINT32 total; //����
	AccountInfo data[1]; //�ʺ���Ϣ


	void hton()
	{
		for (BYTE i = 0; i < retNum; i++)
			data[i].hton();
		total = htonl(total);
	}

	void ntoh()
	{
		for (BYTE i = 0; i < retNum; i++)
			data[i].ntoh();
		total = ntohl(total);
	}
};

///��ѯ�����ʺ���ϸ��Ϣ
struct GMQSingleAccountDetailInofReq
{
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	char name[33]; //�ʺ���

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};


struct GMQSingleAccountDetailInofRes
{
	AccountInfo info;
};



///��ɫ��Ϣ(����web��ѯ)
struct RetActorInfo
{
	BYTE actorType;		//��ɫ����
	BYTE faction;		//������Ӫ
	BYTE sex;			//�Ա�
	BYTE level;			//�ȼ�

	UINT16 occu;			//ְҵ�ǳ�
	UINT16 vist;			//��¼����

	UINT32 actorId;		//��ɫID
	UINT32 accountId;		//�ʺ�ID
	UINT32 hp;			//����ֵ
	UINT32 mp;			//����ħ��ֵ
	UINT32 attack;		//��������ֵ
	UINT32 def;			//��������ֵ
	UINT32 vit;			//����
	UINT32 cp;			//ս��
	UINT32 ap;			//ʣ�����Ե�

	UINT64 gold;		//���
	UINT64 exp;		//����
	UINT64 online;	//����ʱ��

	char createtts[20];	//����ʱ��
	char lvtts[20];		//������ʱ��
	char nick[33];		//�ǳ�

};

///��ɫ��Ϣ
struct ActorInfo
{
	BYTE actorType;		//��ɫ����
	BYTE faction;		//������Ӫ
	BYTE sex;			//�Ա�
	BYTE level;			//�ȼ�

	UINT16 occu;			//ְҵ�ǳ�
	UINT16 vist;			//��¼����

	UINT32 actorId;		//��ɫID
	UINT32 accountId;		//�ʺ�ID
	UINT32 hp;			//����ֵ
	UINT32 mp;			//����ħ��ֵ
	UINT32 attack;		//��������ֵ
	UINT32 def;			//��������ֵ
	UINT32 vit;			//����
	UINT32 cp;			//ս��
	UINT32 ap;			//ʣ�����Ե�

	SBigNumber gold;		//���
	SBigNumber exp;		//����
	SBigNumber online;	//����ʱ��

	char createtts[20];	//����ʱ��
	char lvtts[20];		//������ʱ��
	char nick[33];		//�ǳ�

	void hton()
	{
		occu = htons(occu);
		vist = htons(vist);
		actorId = htonl(actorId);
		accountId = htonl(accountId);
		hp = htonl(hp);
		mp = htonl(mp);
		attack = htonl(attack);
		def = htonl(def);
		vit = htonl(vit);
		cp = htonl(cp);
		ap = htonl(ap);

	}

	void ntoh()
	{
		occu = ntohs(occu);
		vist = ntohs(vist);
		actorId = ntohl(actorId);
		accountId = ntohl(accountId);
		hp = ntohl(hp);
		mp = ntohl(mp);
		attack = ntohl(attack);
		def = ntohl(def);
		vit = ntohl(vit);
		cp = ntohl(cp);
		ap = ntohl(ap);
	}
};

///��ѯ��ɫ����
struct GMQActorDetailInofReq
{

	BYTE to; //��ѯ����
	BYTE type; //��ѯ����(1-����, 2-��¼)
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	UINT32 from; //��ʼλ
	char startTime[20]; //��ʼʱ��
	char endTime[20]; //��ֹʱ��

	void hton()
	{
		platId = htons(platId);
		partId = htons(partId);
		from = htonl(from);
	}

	void ntoh()
	{
		platId = ntohs(platId);
		partId = ntohs(partId);
		from = ntohl(from);
	}
};

struct GMQActorDetailInfoRes
{
	BYTE type; //��ѯ����(1-������2-��¼)
	BYTE retNum; //��������
	UINT32 total; //����

	ActorInfo data[1]; //��ɫ��ϸ��Ϣ

	void hton()
	{
		for (BYTE i = 0; i < retNum; i++)
			data[i].hton();
		total = htonl(total);
	}

	void ntoh()
	{
		for (BYTE i = 0; i < retNum; i++)
			data[i].ntoh();
		total = ntohl(total);
	}
};


///��ѯ������ɫ��ϸ��Ϣ
struct GMQSingleActorDetailInfoReq
{
	UINT16 platId; //����ID
	UINT16 partId; //����ID
	char nick[33]; //��ɫ�ǳ�

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}

};


struct GMQSingleActorDetailInfoRes
{
	ActorInfo info; //��ɫ��Ϣ
};

struct PlatPart
{
	UINT16 platId;		 //ƽ̨ID
	UINT16 platnum;		 //ƽ̨����;
	char platName[33];   //ƽ̨����;

	UINT16 channelid;    //����ID;
	UINT16 channelNum;	 //��������;
	char channelName[33]; //������
	UINT16 partnum;			 //��������
	UINT16 data[1];		 //����ID

	void hton()
	{
		for (int i = 0; i < partnum; i++)
			data[i] = htons(data[i]);
		partnum = htons(partnum);
	}

	void ntoh()
	{
		for (int i = 0; i < partnum; i++)
			data[i] = ntohs(data[i]);
		partnum = ntohs(partnum);
	}

};


///��ȡ����������Ϣ  GM_PLAT_PART_INFO
struct GetPlatPartInfoRes
{
	BYTE num; //����
	PlatPart data[1]; //����������Ϣ

	void hton()
	{
		for (BYTE i = 0; i < num; i++)
			data[i].hton();
	}

	void ntoh()
	{
		for (BYTE i = 0; i < num; i++)
			data[i].ntoh();
	}
};

//ʵʱ����ͳ��;GM_REALTIME_ADDED_STATISTICS 
//======================================================
struct GMRealTimeAddedStatisticsReq
{

	UINT16 platId; //ƽ̨ID
	UINT16 channelId;//����ID
	UINT16 partId; //����ID
	char startTime[20]; //��ѯ��ʼʱ��;
	char endTime[20];	//����ʱ��;

	void hton()
	{
		platId = htons(platId);
		partId = htons(partId);
	}

	void ntoh()
	{
		platId = ntohs(platId);
		partId = ntohs(partId);
	}
};

struct TimeAddedinfo
{
	UINT32 EquipmentAddedNum;		//�豸������;
	UINT32 EquipmentLogInNum;		//�豸��¼��;
	UINT32 EquipmentActiveNum;	//�豸��Ծ��;

	UINT32 AccountAddedNum;		//�˺�������;
	UINT32 AccountLogInNum;		//�˺ŵ�¼��;
	UINT32 AccountActiveNum;		//�˺Ż�Ծ��;

	UINT32 ActorAddedNum;			//��ɫ������;
	UINT32 ActorLogInNum;			//��ɫ��¼��
	UINT32 ActorActiveNum;		//��ɫ��Ծ��

	void hton()
	{
		EquipmentAddedNum = htonl(EquipmentAddedNum);
		EquipmentLogInNum = htonl(EquipmentLogInNum);
		EquipmentActiveNum = htonl(EquipmentActiveNum);
		AccountAddedNum = htonl(AccountAddedNum);
		AccountLogInNum = htonl(AccountLogInNum);
		AccountActiveNum = htonl(AccountActiveNum);
		ActorAddedNum = htonl(ActorAddedNum);
		ActorLogInNum = htonl(ActorLogInNum);
		ActorActiveNum = htonl(ActorActiveNum);
	}

	void ntoh()
	{
		EquipmentAddedNum = ntohl(EquipmentAddedNum);
		EquipmentLogInNum = ntohl(EquipmentLogInNum);
		EquipmentActiveNum = ntohl(EquipmentActiveNum);
		AccountAddedNum = ntohl(AccountAddedNum);
		AccountLogInNum = ntohl(AccountLogInNum);
		AccountActiveNum = ntohl(AccountActiveNum);
		ActorAddedNum = ntohl(ActorAddedNum);
		ActorLogInNum = ntohl(ActorLogInNum);
		ActorActiveNum = ntohl(ActorActiveNum);
	}
};

//��������ͳ�����е�����;
struct GMRealTImeAddedStAllNumRes
{
	TimeAddedinfo InfoNum; //�ֱ����豸���˺ţ���ɫ;
};


//======================================================


//======================================================
///ʵʱ��ֵͳ��;
///��ֵ��Ϣ;
struct RecharegeStatisticeIofo
{
	char	RechargeTime[20];	//��ֵ����;
	char	actorName[33];		//��ɫ��;
	UINT32	actorId;		//�˺�ID��UID;
	UINT16	rmb;			//���,���;
	UINT16	starus;			//��ֵ��ʯ;
	UINT16	platformid;		//��ֵ����;

	void hton()
	{
		actorId = htonl(actorId);
		platformid = htons(platformid);
		starus = htons(starus);
		rmb = htons(rmb);

	}

	void ntoh()
	{
		actorId = ntohl(actorId);
		platformid = ntohs(platformid);
		starus = ntohs(starus);
		rmb = ntohs(rmb);
	}
};

///���󶩵���ѯ;
struct GMGetOrderTrackingRsq
{
	BYTE to;       //��ѯ����
	UINT32 from;		//��ʼλ
	UINT16 platId; //ƽ̨ID
	UINT16 channelId;//����ID
	UINT16 partId; //����ID
	char startTime[20]; //��ʼʱ��
	char endTime[20]; //��ֹʱ��


	void hton()
	{
		platId = htons(platId);
		partId = htons(partId);

	}

	void ntoh()
	{
		platId = ntohs(platId);
		partId = ntohs(partId);
	}
};
///������ѯ������Ϣ;	
struct GMGetRealtimeRechargeStatisticsRes
{
	BYTE retNum; //��ǰ������;
	UINT32 total; //����;
	RecharegeStatisticeIofo data[1];//��ֵ��Ϣ;

	void hton()
	{
		for (int i = 0; i < retNum; i++)
			data[i].hton();
	}

	void ntoh()
	{
		for (int i = 0; i < retNum; i++)
			data[i].ntoh();
	}
};

///�����ֵ����ͳ��;
struct GMGetRechargeStatisticsRsq
{

	UINT16 platId; //ƽ̨ID
	UINT16 channelId;//����ID
	UINT16 partId; //����ID
	BYTE to;       //��������
	UINT32 from;		//��ʼ������0��ΪstartTime�����Ϊ���֣����˺�������ʼ��ѯ;
	char startTime[20]; //��ѯ��ʼʱ��;
	char endTime[20];	//����ʱ��;

	void hton()
	{
		platId = htons(platId);
		partId = htons(partId);

	}

	void ntoh()
	{
		platId = ntohs(platId);
		partId = ntohs(partId);
	}
};
struct StartisticsInfo
{
	char   dateTime[20];			//��ѯ������;
	UINT32 PaymentUserNum;		//�����û���;
	UINT32 newPaymentUserNum;	//���������û���;
	UINT32 PaymentcountNum;		//���Ѵ���;
	UINT32 PaymentMoney;		//���ѽ��;

	float LoginPaymentRates;	//��¼������;
	float RegiPaymentRates;		//ע�Ḷ����;
	float PaymentARPU;			//����ARPU;
	float LoginARPU;			//��¼ARPU;

	void hton()
	{
		PaymentUserNum = htonl(PaymentUserNum);
		newPaymentUserNum = htonl(newPaymentUserNum);
		LoginPaymentRates = htonl(LoginPaymentRates);
		RegiPaymentRates = htonl(RegiPaymentRates);
	}

	void ntoh()
	{
		PaymentUserNum = ntohl(PaymentUserNum);
		newPaymentUserNum = ntohl(newPaymentUserNum);
		LoginPaymentRates = ntohl(LoginPaymentRates);
		RegiPaymentRates = ntohl(RegiPaymentRates);
	}
};
struct GMGetRecharStatisticsRes
{
	BYTE retNum; //��ǰ������;
	UINT32 total; //����;
	StartisticsInfo Rechardata[1];

};

//======================================================

//======================================================
///��ѯ������������;��ͳ���������ݣ�
struct GMQLeaveKeepReq
{
	UINT16 platId; //ƽ̨ID
	UINT16 channelId;//����ID
	UINT16 partId; //����ID
	char startTime[20];		//��ѯʱ��;
	char entTime[20];		//����ʱ��;
	BYTE lktype;			//��������[1-�ʺ�����, 2-�豸����, 3-��ɫ����]
	BYTE lkTimeType;		//����ʱ������[1-��������, 3-����������, 7-����������, 30-��30������, 60-������](����ʱ������������leavekeep����)

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
	}
};

struct GMQLeavekeepDateInfo		//��ѯ��ĳ������棬��GMQLeaveKeepInfo�����������ĿǰΪֹΪ1��3��7��30��60��; �ֱ�Ϊ1��2��3��4��5���ṹ��;
{
	UINT16 retNum;				//������;���ؼ�����ȡLeaveDate����������û��ϵ;
	char LeaveTime[20];			//����ʱ��,��ʼʱ��;
	UINT32 NewRecount;			//��ѯ�յĵ�¼����;
	UINT16 Retained[10];		//������(��¼����);//��������������,1,3,7,30,60;	
	float  rate[10];					//������;	//��������������,1,3,7,30,60;

	void hton()
	{
		NewRecount = htonl(NewRecount);
		retNum = htons(retNum);
	}

	void ntoh()
	{
		NewRecount = ntohl(NewRecount);
		retNum = ntohs(retNum);
	}

};
struct GMQLeaveKeepRes
{

	UINT32 retTimeNum;					//��ѯ����������;	
	GMQLeavekeepDateInfo KeepTime[1];	//��ѯĳһ�������;

};

//======================================================

/*�����û�ͳ��;*/
//======================================================

///ʵʱ���� GM_ONLINE_REALTIME 0xC371 ��̨���ݳ���ʱ����Ϊ10���ӣ�ÿ��144������
struct GMOnlineCountReq
{
	UINT16 platId; //ƽ̨ID
	UINT16 channelId;//����ID
	UINT16 partId; //����ID
	BYTE to;			//��������
	UINT32 from;		//��ʼ������0��ΪstartTime�����Ϊ���֣����˺�������ʼ��ѯ;
	char startTime[20];  //��ѯ����(��ѡ����Ϊ����ʵʱ����)��Ҫ��ѯ�Ƿ�Ϊ���죬��Ȼ��Ϊ������һ���Է���144������;���Ϊ�����򷵻ش�00:10:00��ʼ��������-24:00:00;,��ѯʱ��Ϊ���һ����ѯ��ʱ��;��ʼΪ0;						//��ϵͳ��ǰʱ��Ϊ��ѯ����ʱ��;
	void hton()
	{
		int offset = 0;
		offset += sizeof(char);
		offset += sizeof(UINT16);
		offset += sizeof(UINT32);
	}

	void ntoh()
	{
		int offset = 0;
		offset += sizeof(char);
		offset += sizeof(UINT16);
		offset += sizeof(UINT32);
	}
};
struct GMOnlineCountRes
{
	BYTE retNum;		  //��ǰ��������,���Ϊ���������Ϊ144��,����ȡ144��info;
	UINT16 total;         //����	//һ�췵����������һ��144������;
	OnlineInfo info[1];   //������Ϣ 


};

///��������ͳ��;
struct GMOnlineDateStatisticsReq
{
	UINT16	platId;		//����
	UINT16	partId;		//����;
	char startTime[20]; //��ʼʱ��
	char endTime[20]; //��ֹʱ��

	void hton()
	{
		platId = htons(platId);
		partId = htons(partId);

	}

	void ntoh()
	{
		platId = ntohs(platId);
		partId = ntohs(partId);
	}
};
struct GMOnlineDateStatisticsInfo
{
	char	dayTime[20];//����;
	UINT16 userLogInNum;//��¼�û���;
	UINT16 logInNum;//��¼�ܴ���;
	UINT16 averageLogInNum;//ƽ����¼����;
	UINT32 onlinesumTime;//������ʱ��;
	UINT32 onlineaverageTime;//ƽ������ʱ��;
	UINT32 onlineaverageAcu;//ACU�������˾���ֵ��
	UINT32 onlineaveragePcu;//PCU(�����˾���ֵ)
	UINT32 onlineaveragePCU_ACU;//PCU/ACU

	void hton()
	{
		userLogInNum = htons(userLogInNum);
		logInNum = htons(logInNum);
		averageLogInNum = htons(averageLogInNum);
		onlinesumTime = htonl(onlinesumTime);
		onlineaverageTime = htonl(onlineaverageTime);
		onlineaverageAcu = htonl(onlineaverageAcu);
		onlineaveragePcu = htonl(onlineaveragePcu);
		onlineaveragePCU_ACU = htonl(onlineaveragePCU_ACU);

	}

	void ntoh()
	{
		userLogInNum = ntohs(userLogInNum);
		logInNum = ntohs(logInNum);
		averageLogInNum = ntohs(averageLogInNum);
		onlinesumTime = ntohl(onlinesumTime);
		onlineaverageTime = ntohl(onlineaverageTime);
		onlineaverageAcu = ntohl(onlineaverageAcu);
		onlineaveragePcu = ntohl(onlineaveragePcu);
		onlineaveragePCU_ACU = ntohl(onlineaveragePCU_ACU);
	}

};
struct GMOnlineDateStatisticsRes
{
	BYTE retNum;		  //��������;
	GMOnlineDateStatisticsInfo date[1];
};


//======================================================

//======================================================


/*�ȼ��ֲ�ͳ��*/
//======================================================
struct GM_Grade_RatingStatisticsRsq
{
	UINT16 platId;		//ƽ̨ID
	UINT16 channelId;		//����ID
	UINT16 partId;		//����ID
	char startTime[20]; //��ʼʱ��
	char endTime[20]; //��ֹʱ��

	void hton()
	{
		platId = htons(platId);
		partId = htons(partId);

	}

	void ntoh()
	{
		platId = ntohs(platId);
		partId = ntohs(partId);
	}

};

struct GM_Grade_RatingStatisticsInfo
{
	UINT16 grade;		//�ȼ����ȼ��ֲ���;
	UINT16 acterNum;	//�˵ȼ���ɫ������;
	float  proportion;	//�����н�ɫ��ӵ�е������ٷֱ�;

	void hton()
	{
		grade = htons(grade);
		acterNum = htons(acterNum);

	}

	void ntoh()
	{
		grade = ntohs(grade);
		acterNum = ntohs(acterNum);
	}
};
struct GM_Grade_RatingStatisticsRes
{
	BYTE retNum;		  //��������;//ÿ���ȼ�һ��info;
	UINT32 Grade_sum;		//�ܼ�;
	GM_Grade_RatingStatisticsInfo date[1];

};

//======================================================

#pragma pack(pop)

#endif