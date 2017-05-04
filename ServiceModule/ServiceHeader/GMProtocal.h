#ifndef __GM_PROTOCAL_H__
#define __GM_PROTOCAL_H__

#include "cyaBase.h"
#include "ServiceCommon.h"

#define DEFAULT_FILE_SIZE	4*1024	//默认文件大小为4M
#define DEFAULT_MAX_COUNT	288		//每日最大保存在线人数次数
#define IS_RUN_STORE		0		//是否开启写入数据模式
#define IS_SCREEN_LOG		1		//是否开启显示日志至屏幕模式
#define IS_RUN_STORE_LK		1		//是否开启留存数据存入db模式

#define LK_ACCOUNT		"account_leavekeep"		//账号留存表
#define LK_DEVICE		"device_leavekeep"		//设备留存表
#define LK_ACTOR		"actor_leavekeep"		//角色留存表
#define TB_ACCOUNT		"account"				//账号表
#define TB_ACTOR		"actor"					//角色表
#define TB_CASH			"cash"					//充值表

#define ACCOUNT_INFO		"regist_account"		//注册账号信息表
#define DEVICE_INFO			"active_device"			//注册设备表（只保存注册设备的数量）
#define ACTOR_INFO			"creat_actor"			//创建角色信息表
#define ACCOUNT_LOGIN_INFO	"account_info"			//账号信息（记录注册和登陆的账号信息）

#define DB_SGS				"sgs"				//sgs db
#define DB_SGSINFO			"sgsinfo"			//sgsinfo db
#define DB_SGSVERIFY		"sgsverify"			//sgsverify db
#define DB_SGSRECHARGE		"sgsrecharge"		//sgsrecharge db
#define DB_SGS_ACTION		"sgs_action"		//sgs_action db

#define IS_TEST 0	//测试

///留存类型
enum LeaveKeepType
{
	ACCOUNT_LK = 1, //帐号留存
	DEVICE_LK = 2, //设备留存
	ACTOR_LK = 3  //角色留存
};

///留存时间类型
enum LKTimeType
{
	MORROW_LK = 1, //次日留存
	THIRD_LK = 3, //三日留存
	SEVENTH_LK = 7, //七日留存
	THIRTYTH_LK = 30, //第三十日留存
	MONTH_LK = 90	 //月留存
};

///平均在线类型
enum AverageOlPerType
{
	OL_PER_SECOND,  //每秒在线
	OL_PER_MINUTE,  //每分在线
	OL_PER_HOUR,	  //每小时在线
	OL_PER_DAY,    //每天在线
	OL_PER_MONTH,  //每月在线
	OL_PER_YEAR    //每年在线
};

///在线人数类型
enum OnlineType
{
	OL_COMM = 1,     //每日定时刷新在线人数
	OL_MAX = 2,     //每日最大在线人数
	OL_AVERAGE = 3		//每日平均在线人数
};

///充值类型
///首充用户数需要单独处理
enum CashType
{
	PY_CASH_ACTOR = 1, //当日付费角色数
	PY_CASH_COUNT = 2, //当日付费总次数
	PY_CASH_TOTAL = 3, //累计新增付费用户数
	PY_CASH_NRMB = 4, //当日新增付费用户的付费金额
	PY_CASH_CRMB = 5, //当日累计付费金额
	PY_CASH_TORMB = 6, //累计付费用户（开服起）
};

enum FuncOperateType
{
	ACCOUNT_CREATE = 1, //账号创建数
	ADD_NEW_USER = 2,   //新增用户数
	LOGIN_USER = 3      //登录用户数
};

#pragma pack(push, 1)
///GMServer 通信协议头
struct GMProtocalHead
{
	UINT16 pduLen : 12;	 //数据长度
	UINT16 pktType : 3;	 //包类型
	UINT16 isCrypt : 1;	 //是否加密
	UINT32 req;		 //包序列号
	BYTE isOver;      //是否为完整包

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

///连接后发送的key和心跳间隔时间
struct GMConntedTokenHeart
{
	UINT16 heatbeatInterval;  //心跳时间间隔
	char key[33];				//密钥

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

///管理员登录 GM_USER_LOGIN
struct GMUserLoginReq
{
	char name[33];    //用户名
	char passwd[33];  //密码
};

///激活设备信息
struct ActiveDevInfo
{
	char devIp[16];		//设备IP
	char tts[20];			//激活时间
	char devType[33];		//机型
	char sys[33];			//系统
	char devIdfa[41];		//设备号IDFA
	char region[41];		//地域
};

///设备激活 GM_DEVICE_ACTIVE
struct GMQActiveDeviceReq
{
	BYTE from;			//起始
	BYTE to;				//查询记录数
	char startTime[20];	//起始时间
	char endTime[20];		//起始时间
};

struct GMQActiveDeviceRes
{
	BYTE retNum;			 //当前返回数量
	UINT32 total;			 //总数
	ActiveDevInfo data[1]; //设备信息

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

///新增设备 GM_ACCOUNT_CREATE
struct GMQAddNewDeviceReq
{
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	char startTime[20];	//查询起始时间
	char endTime[20];		//查询起始时间
	BYTE stype;			//(1-当日新增(不含未创角色)，2-当日新增总人数, 3-截止当日创建总的帐号数)

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
	UINT32 count; //新增设备数
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

///帐号创建数 GM_ACCOUNT_CREATE
struct GMQCreateAccountReq
{
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	char startTime[20];	//查询起始时间
	char endTime[20];		//查询截止时间
	BYTE stype;			//创建类型(1-当日新增(不含未创角色)，2-当日新增总人数, 3-截止当日创建总的帐号数)

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
	UINT32 count; //帐号创建数
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

///登录数 GM_LOGIN_ACCOUNT
struct GMLoginQueryReq
{
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	char startTime[20];	//查询起始时间
	char endTime[20];		//查询截止时间
	BYTE stype;			//登录类型(1-登录用户, 2-登录设备, 3-登录角色)

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
	UINT32 count; //登录数量
	BYTE stype;   //登录类型(1-登录用户, 2-登录设备, 3-登录角色)
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

/// 新增数 GM_ADD_NEW_ACCOUNT
struct GMQAddNewAccountReq
{
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	char startTime[20];	//查询起始时间
	char endTime[20];		//查询截止时间
	BYTE stype;			//新增类型(1-新增用户, 2-新增设备, 3-新增角色)

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
	UINT32 count; //帐号创建数
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

/// 活跃数 GM_ACTIVE_ACCOUNT
struct GMQActiveUserReq
{
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	char startTime[20];	//查询起始时间
	char endTime[20];		//查询截止时间
	BYTE stype;			//类型(1-活跃用户数, 2-活跃设备数,3-活跃角色)

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
	UINT32 count; //帐号创建数
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

/// 累计总数 GM_ADDUP_NEW
struct GMQAddupNewReq
{
	UINT16 platId;		//平台ID
	UINT16 channelId;		//渠道ID
	UINT16 partId;		//分区ID

	char startTime[20];	//查询起始时间
	char endTime[20];		//查询截止时间
	BYTE stype;			//累计类型(1-累计用户, 2-累计设备, 3-累计角色)

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
	UINT32 count; //累计总数
	BYTE stype;  //类型1:用户,2,设备,3:角色

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

///在线信息——————
struct OnlineInfo
{
	UINT32 onlineCount; //在线人数
	char olTime[20];    //当前在线时间

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

///平均游戏时间
struct GMQAverageTimeReq
{
	UINT16 platId;		//平台ID
	UINT16 channelId;		//渠道ID
	UINT16 partId;		//分区ID

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

///平均在线时间
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

///平均游戏次数
struct GMQAveragePlayGameReq
{
	UINT16 platId;		//平台ID
	UINT16 channelId;		//渠道ID
	UINT16 partId;		//分区ID

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

///平均游戏次数
struct GMQAveragePlayGameRes
{
	UINT16 averagePlayCount; //平均游戏次数

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

///最大在线人数 GM_MAX_ONLINE_USER_COUNT
struct GMMaxOnlineReq
{

	BYTE to;				    //查询数量
	UINT16 platId;				//渠道ID
	UINT16 partId;				//分区ID
	UINT32 from;			    //起始值
	char startTime[20];			//起始时间
	char endTime[20];			//结束时间

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
	BYTE retNum;			//返回数量
	UINT32 total;			//总数 
	OnlineInfo info[1];	//在线信息 

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

///平均在线人数 GM_AVERAGE_ONLINE_USER_COUNT
struct GMAverageOnlineReq
{
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	char startTime[20];  //起始时间
	char endTime[20];    //结束时间

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
	UINT32 onlineNum; //平均在线人数
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

///付费用户数 GM_PAYMENT_USER_COUNT
struct GMPaymentUserReq
{
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	char startTime[20];  //起始时间
	char endTime[20];    //结束时间

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
	UINT32 paymentCount; //付费用户数

	void hton()
	{
		paymentCount = htonl(paymentCount);
	}

	void ntoh()
	{
		paymentCount = ntohl(paymentCount);
	}
};

///付费次数 GM_PAYMENT_COUNT
struct GMPaymentCountReq
{
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	char startTime[20];  //起始时间
	char endTime[20];    //结束时间

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
	UINT32 paymentCount; //付费次数

	void hton()
	{
		paymentCount = htonl(paymentCount);
	}

	void ntoh()
	{
		paymentCount = ntohl(paymentCount);
	}
};

///新增付费用户数 GM_PAYMENT_USER_COUNT
struct GMFisrstRechargeReq
{
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	char startTime[20];  //起始时间
	char endTime[20];    //结束时间

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
	UINT32 firstPayCount; //首充次数

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

///累计新增付费用户数 GM_ADDUP_PAYMENT_COUNT
struct GMAddupPaymentReq
{
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	char startTime[20];  //起始时间
	char endTime[20];    //结束时间

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
	UINT32 addupPayment; //累计新增付费用户数

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

///新增付费金额 GM_ADD_NEW_PAYMENT_MONEY
struct GMAddNewPaymentMoneyReq
{
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	char startTime[20];  //起始时间
	char endTime[20];    //结束时间

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
	UINT32 addNewPaymentMoney; //新增付费金额

	void hton()
	{
		addNewPaymentMoney = htonl(addNewPaymentMoney);
	}

	void ntoh()
	{
		addNewPaymentMoney = ntohl(addNewPaymentMoney);
	}
};

///付费金额 GM_PAYMENT_MONEY
struct GMPaymentMoneyReq
{
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	char startTime[20];  //起始时间
	char endTime[20];    //结束时间

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
	UINT32 paymentMoney; //付费金额

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


///累计付费金额 GM_ADDUP_PAYMENT_MONEY
struct GMPaymentTotalMoneyReq
{
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	char startTime[20];  //起始时间
	char endTime[20];    //结束时间

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
	UINT32 paymentMoney; //累计付费金额

	void hton()
	{
		paymentMoney = htonl(paymentMoney);
	}

	void ntoh()
	{
		paymentMoney = ntohl(paymentMoney);
	}
};


///消费信息
struct CostInfo
{
	BYTE type;			//消费类型
	UINT32 actorId;		//角色ID
	UINT32 money;			//消费金额
	UINT32 remain;		//余额
	char time[20];		//消费时间
	char nick[33];		//昵称
	char action[128];		//消费行为

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


///消费总金额数
struct GMQCostTotalReq
{
	BYTE to;			//查询数量
	BYTE costType;		//消费类型(0-金币， 1-代币)
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	UINT32 from;		//起始值
	char startTime[20];	//起始时间
	char endTime[20];		//截至时间

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
	BYTE retNum;			//返回数量
	UINT32 total;			//总数
	UINT32 costMoney;		//消费总金额
	CostInfo data[1];		//消费详情


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

///单个角色总消费
struct GMSingleActorCostReq
{
	BYTE costType;		//消费类型(0-金币， 1-代币)
	BYTE to;				//查询数量
	UINT16 platId;		//渠道ID
	UINT16 partId;		//分区ID
	UINT32 from;			//查询起始位置
	char nick[33];		//角色昵称
	char startTime[20];	//起始时间
	char endTime[20];		//截至时间

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
	BYTE retNum;			//返回数量
	UINT32 total;			//总数
	UINT32 costMoney;		//消费总金额
	CostInfo data[1];		//消费详情


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


///帐号信息
struct AccountInfo
{
	BYTE status; //状态
	UINT16 partId; //分区ID
	UINT32 id; //帐号ID
	UINT32 visit; //登录次数
	char ip[16]; //注册IP
	char platsource[17]; //注册渠道
	char os[17]; //用户注册使用操作系统
	char model[17]; //机型
	char ver[17]; //当前包版本
	char regtts[20]; //注册时间
	char lasttts[20]; //最好一次登录时间
	char name[33]; //用户名
	char passwd[33]; //密码
	char regdev[65]; //注册设备号
	char lastdev[65]; //最好一次登录设备号

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

///查询帐号详情
struct GMQAccountDetailInofReq
{

	BYTE to; //查询数量 
	BYTE type; //类型(1-创建, 2-登录)
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	UINT32 from; //起始位
	char startTime[20]; //起始时间
	char endTime[20]; //终止时间

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
	BYTE type; //查询类型(1-注册，2-登录)
	BYTE retNum; //当前返回数
	UINT32 total; //总数
	AccountInfo data[1]; //帐号信息


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

///查询单个帐号详细信息
struct GMQSingleAccountDetailInofReq
{
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	char name[33]; //帐号名

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



///角色信息(返回web查询)
struct RetActorInfo
{
	BYTE actorType;		//角色类型
	BYTE faction;		//所在阵营
	BYTE sex;			//性别
	BYTE level;			//等级

	UINT16 occu;			//职业昵称
	UINT16 vist;			//登录次数

	UINT32 actorId;		//角色ID
	UINT32 accountId;		//帐号ID
	UINT32 hp;			//生命值
	UINT32 mp;			//基础魔力值
	UINT32 attack;		//基础攻击值
	UINT32 def;			//基础防御值
	UINT32 vit;			//体力
	UINT32 cp;			//战力
	UINT32 ap;			//剩余属性点

	UINT64 gold;		//金币
	UINT64 exp;		//经验
	UINT64 online;	//在线时间

	char createtts[20];	//创建时间
	char lvtts[20];		//最后访问时间
	char nick[33];		//昵称

};

///角色信息
struct ActorInfo
{
	BYTE actorType;		//角色类型
	BYTE faction;		//所在阵营
	BYTE sex;			//性别
	BYTE level;			//等级

	UINT16 occu;			//职业昵称
	UINT16 vist;			//登录次数

	UINT32 actorId;		//角色ID
	UINT32 accountId;		//帐号ID
	UINT32 hp;			//生命值
	UINT32 mp;			//基础魔力值
	UINT32 attack;		//基础攻击值
	UINT32 def;			//基础防御值
	UINT32 vit;			//体力
	UINT32 cp;			//战力
	UINT32 ap;			//剩余属性点

	SBigNumber gold;		//金币
	SBigNumber exp;		//经验
	SBigNumber online;	//在线时间

	char createtts[20];	//创建时间
	char lvtts[20];		//最后访问时间
	char nick[33];		//昵称

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

///查询角色详情
struct GMQActorDetailInofReq
{

	BYTE to; //查询数量
	BYTE type; //查询类型(1-创建, 2-登录)
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	UINT32 from; //起始位
	char startTime[20]; //起始时间
	char endTime[20]; //终止时间

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
	BYTE type; //查询类型(1-创建，2-登录)
	BYTE retNum; //返回数量
	UINT32 total; //总数

	ActorInfo data[1]; //角色详细信息

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


///查询单个角色详细信息
struct GMQSingleActorDetailInfoReq
{
	UINT16 platId; //渠道ID
	UINT16 partId; //分区ID
	char nick[33]; //角色昵称

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
	ActorInfo info; //角色信息
};

struct PlatPart
{
	UINT16 platId;		 //平台ID
	UINT16 platnum;		 //平台数量;
	char platName[33];   //平台名字;

	UINT16 channelid;    //渠道ID;
	UINT16 channelNum;	 //渠道数量;
	char channelName[33]; //渠道名
	UINT16 partnum;			 //分区数量
	UINT16 data[1];		 //分区ID

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


///获取渠道分区信息  GM_PLAT_PART_INFO
struct GetPlatPartInfoRes
{
	BYTE num; //数量
	PlatPart data[1]; //渠道分区信息

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

//实时新增统计;GM_REALTIME_ADDED_STATISTICS 
//======================================================
struct GMRealTimeAddedStatisticsReq
{

	UINT16 platId; //平台ID
	UINT16 channelId;//渠道ID
	UINT16 partId; //分区ID
	char startTime[20]; //查询开始时间;
	char endTime[20];	//结束时间;

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
	UINT32 EquipmentAddedNum;		//设备新增数;
	UINT32 EquipmentLogInNum;		//设备登录数;
	UINT32 EquipmentActiveNum;	//设备活跃数;

	UINT32 AccountAddedNum;		//账号新增数;
	UINT32 AccountLogInNum;		//账号登录数;
	UINT32 AccountActiveNum;		//账号活跃数;

	UINT32 ActorAddedNum;			//角色新增数;
	UINT32 ActorLogInNum;			//角色登录数
	UINT32 ActorActiveNum;		//角色活跃数

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

//返回新增统计所有的数量;
struct GMRealTImeAddedStAllNumRes
{
	TimeAddedinfo InfoNum; //分别是设备，账号，角色;
};


//======================================================


//======================================================
///实时充值统计;
///充值信息;
struct RecharegeStatisticeIofo
{
	char	RechargeTime[20];	//充值日期;
	char	actorName[33];		//角色名;
	UINT32	actorId;		//账号ID，UID;
	UINT16	rmb;			//额度,金额;
	UINT16	starus;			//充值钻石;
	UINT16	platformid;		//充值渠道;

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

///请求订单查询;
struct GMGetOrderTrackingRsq
{
	BYTE to;       //查询数量
	UINT32 from;		//起始位
	UINT16 platId; //平台ID
	UINT16 channelId;//渠道ID
	UINT16 partId; //分区ID
	char startTime[20]; //起始时间
	char endTime[20]; //终止时间


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
///订单查询返回信息;	
struct GMGetRealtimeRechargeStatisticsRes
{
	BYTE retNum; //当前返还数;
	UINT32 total; //总数;
	RecharegeStatisticeIofo data[1];//充值信息;

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

///请求充值数据统计;
struct GMGetRechargeStatisticsRsq
{

	UINT16 platId; //平台ID
	UINT16 channelId;//渠道ID
	UINT16 partId; //分区ID
	BYTE to;       //结束天数
	UINT32 from;		//起始天数，0则为startTime，如果为数字，则退后天数开始查询;
	char startTime[20]; //查询开始时间;
	char endTime[20];	//结束时间;

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
	char   dateTime[20];			//查询的日期;
	UINT32 PaymentUserNum;		//付费用户数;
	UINT32 newPaymentUserNum;	//新增付费用户数;
	UINT32 PaymentcountNum;		//付费次数;
	UINT32 PaymentMoney;		//付费金额;

	float LoginPaymentRates;	//登录付费率;
	float RegiPaymentRates;		//注册付费率;
	float PaymentARPU;			//付费ARPU;
	float LoginARPU;			//登录ARPU;

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
	BYTE retNum; //当前返还数;
	UINT32 total; //总数;
	StartisticsInfo Rechardata[1];

};

//======================================================

//======================================================
///查询渠道分区留存;（统计留存数据）
struct GMQLeaveKeepReq
{
	UINT16 platId; //平台ID
	UINT16 channelId;//渠道ID
	UINT16 partId; //分区ID
	char startTime[20];		//查询时间;
	char entTime[20];		//结束时间;
	BYTE lktype;			//留存类型[1-帐号留存, 2-设备留存, 3-角色留存]
	BYTE lkTimeType;		//留存时间类型[1-次日留存, 3-第三日留存, 7-第七日留存, 30-第30日留存, 60-月留存](留存时间类型由配置leavekeep决定)

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

struct GMQLeavekeepDateInfo		//查询的某天的留存，及GMQLeaveKeepInfo留存的天数，目前为止为1，3，7，30，60天; 分别为1，2，3，4，5个结构体;
{
	UINT16 retNum;				//返回数;返回几，就取LeaveDate几，跟天数没关系;
	char LeaveTime[20];			//留存时间,开始时间;
	UINT32 NewRecount;			//查询日的登录人数;
	UINT16 Retained[10];		//留存数(登录人数);//数据留存率天数,1,3,7,30,60;	
	float  rate[10];					//留存率;	//数据留存率天数,1,3,7,30,60;

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

	UINT32 retTimeNum;					//查询区间总天数;	
	GMQLeavekeepDateInfo KeepTime[1];	//查询某一天的留存;

};

//======================================================

/*在线用户统计;*/
//======================================================

///实时在线 GM_ONLINE_REALTIME 0xC371 后台数据呈现时间间隔为10分钟，每天144个数据
struct GMOnlineCountReq
{
	UINT16 platId; //平台ID
	UINT16 channelId;//渠道ID
	UINT16 partId; //分区ID
	BYTE to;			//结束天数
	UINT32 from;		//起始天数，0则为startTime，如果为数字，则退后天数开始查询;
	char startTime[20];  //查询日期(不选择则为当日实时在线)需要查询是否为当天，不然不为当天则一次性返回144个数据;如果为当天则返回从00:10:00开始返回数据-24:00:00;,查询时间为最后一个查询的时间;初始为0;						//以系统当前时间为查询结束时间;
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
	BYTE retNum;		  //当前返回数量,如果为当天则次数为144个,就是取144个info;
	UINT16 total;         //总数	//一天返回数据总量一共144个数据;
	OnlineInfo info[1];   //在线信息 


};

///在线数据统计;
struct GMOnlineDateStatisticsReq
{
	UINT16	platId;		//渠道
	UINT16	partId;		//分区;
	char startTime[20]; //起始时间
	char endTime[20]; //终止时间

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
	char	dayTime[20];//日期;
	UINT16 userLogInNum;//登录用户数;
	UINT16 logInNum;//登录总次数;
	UINT16 averageLogInNum;//平均登录次数;
	UINT32 onlinesumTime;//在线总时长;
	UINT32 onlineaverageTime;//平均在线时长;
	UINT32 onlineaverageAcu;//ACU（在线人均均值）
	UINT32 onlineaveragePcu;//PCU(在线人均峰值)
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
	BYTE retNum;		  //返回数据;
	GMOnlineDateStatisticsInfo date[1];
};


//======================================================

//======================================================


/*等级分布统计*/
//======================================================
struct GM_Grade_RatingStatisticsRsq
{
	UINT16 platId;		//平台ID
	UINT16 channelId;		//渠道ID
	UINT16 partId;		//分区ID
	char startTime[20]; //起始时间
	char endTime[20]; //终止时间

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
	UINT16 grade;		//等级（等级分布）;
	UINT16 acterNum;	//此等级角色的数量;
	float  proportion;	//在所有角色中拥有的数量百分比;

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
	BYTE retNum;		  //返回数据;//每个等级一个info;
	UINT32 Grade_sum;		//总计;
	GM_Grade_RatingStatisticsInfo date[1];

};

//======================================================

#pragma pack(pop)

#endif