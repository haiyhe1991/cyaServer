#ifndef __SERVICE_RANK_H__
#define __SERVICE_RANK_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SGS_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif //SGS_UNREAL_BUILD

/***********************************************************************************
����������ݽṹ
***********************************************************************************/
#define MAX_RANK_MSG_BYTES  1024    //������Ϣ��ѯ��Ӧ��Ϣ��󳤶�
#define MAX_RANK_NUM        100     //���а���ʾ��¼��
#define RANK_NICK_LEN       32

#pragma pack(push, 1)

//������Ϣ��ѯ����
enum ERankType
{
	RANK_ACTOR_LEVEL = 1,   // ��ɫ�ȼ�
	RANK_ACTOR_CP = 2,   // ��ɫս����
	RANK_ACTOR_DUPLICATE = 3,   // ��ɫ����
	RANK_ACTOR_POINT = 4,   // ��ɫ�ɾ͵���
	RANK_ACTOR_COMP = 5,   // ��ɫ����ģʽ
	RANK_ACTOR_TYPE_MAX
};

// ������Ϣ��ѯ����, RANK_QUERY_ROLE_INFO
struct SGetRankInfoReq
{
	BYTE    type;               // ���в�ѯ����
	BYTE    num;                // һ�β�ѯ��������Ŀ
	UINT32  start;              // ��ѯ��ʼλ��(�ӵڼ�����ʼ��ѯ, ֵΪ[0~n]*num)

	void hton(){ start = htonl(start); }
	void ntoh(){ start = ntohl(start); }
};

// ��ѯ��ɫ��ĳ������(�ȼ�/ս����/����), RANK_QUERY_ROLE_RANK
struct SGetRoleRankInfoReq
{
	BYTE    querytype;          // ���в�ѯ����
	UINT32  roleId;             // ��ɫID

	void hton(){ roleId = htonl(roleId); }
	void ntoh(){ roleId = ntohl(roleId); }
};

// ������Ϣ��ѯ���
struct SGetRankInfoResult
{
	BYTE    type;               // ���в�ѯ����
	BYTE    num;                // һ�β�ѯ����ļ�¼��Ŀ
};

// ��ɫ�ȼ���ѯ�����Ϣ
struct SRoleLevelAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // ��ɫ����
	char    roleNick[RANK_NICK_LEN + 1];  // ��ɫ�ǳ�
	BYTE    level;                      // ��ɫ�ȼ�

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
	}
	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
	}
};

// ��ɫս������ѯ�����Ϣ
struct SRoleCPAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // ��ɫ����
	char    roleNick[RANK_NICK_LEN + 1];  // ��ɫ�ǳ�
	UINT32  cp;                         // ��ɫս����

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
		cp = htonl(cp);
	}

	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
		cp = ntohl(cp);
	}
};

// ��ɫ������ѯ�����Ϣ
struct SRoleInstAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // ��ɫ����
	char    roleNick[RANK_NICK_LEN + 1];  // ��ɫ�ǳ�
	UINT16  instId;                     // ��ɫ����

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
		instId = htons(instId);
	}

	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
		instId = ntohs(instId);
	}
};

// ��ɫ�ɾ͵�����ѯ�����Ϣ
struct SRolePointAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // ��ɫ����
	char    roleNick[RANK_NICK_LEN + 1];  // ��ɫ�ǳ�
	UINT32  point;                      // ��ɫ�ɾ͵���

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
		point = htonl(point);
	}

	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
		point = ntohl(point);
	}
};

// ��ɫ����ģʽ��ѯ�����Ϣ
struct SRoleCompAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // ��ɫ����
	char    roleNick[RANK_NICK_LEN + 1];  // ��ɫ�ǳ�
	UINT32  competitive;                // ��ɫ����ģʽ

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
		competitive = htonl(competitive);
	}

	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
		competitive = ntohl(competitive);
	}
};

// ��ɫ�ȼ�������ѯ���
struct SRoleLevelRank
{
	BYTE    level;                      // ��ɫ�ȼ�
	UINT32  level_rank;                 // ��ɫ�ȼ�����

	void hton()
	{
		level_rank = htonl(level_rank);
	}

	void ntoh()
	{
		level_rank = ntohl(level_rank);
	}
};

// ��ɫս����������ѯ���
struct SRoleCPRank
{
	UINT32  cp;                         // ��ɫս����
	UINT32  cp_rank;                    // ��ɫս��������

	void hton()
	{
		cp = htonl(cp);
		cp_rank = htonl(cp_rank);
	}

	void ntoh()
	{
		cp = ntohl(cp);
		cp_rank = ntohl(cp_rank);
	}
};

// ��ɫ����������ѯ���
struct SRoleInstRank
{
	UINT16  instId;                     // ��ɫ����
	UINT32  inst_rank;                  // ��ɫ��������

	void hton()
	{
		instId = htons(instId);
		inst_rank = htonl(inst_rank);
	}

	void ntoh()
	{
		instId = ntohs(instId);
		inst_rank = ntohl(inst_rank);
	}
};

// ��ɫ�ɾ͵���������ѯ���
struct SRolePointRank
{
	UINT32  point;                      // ��ɫ�ɾ͵���
	UINT32  point_rank;                 // ��ɫ�ɾ͵�������

	void hton()
	{
		point = htonl(point);
		point_rank = htonl(point_rank);
	}

	void ntoh()
	{
		point = ntohl(point);
		point_rank = ntohl(point_rank);
	}
};

// ��ɫ����ģʽ������ѯ���
struct SRoleCompRank
{
	UINT32  competitive;                // ��ɫ����ģʽ
	UINT32  comp_rank;                  // ��ɫ����ģʽ����

	void hton()
	{
		competitive = htonl(competitive);
		comp_rank = htonl(comp_rank);
	}

	void ntoh()
	{
		competitive = ntohl(competitive);
		comp_rank = ntohl(comp_rank);
	}
};

// ��ɫ�ȼ�������Ϣ��ѯ���
struct SGetRoleLevelRes
{
	SGetRankInfoResult  result;     // ���в�ѯ���ͺͼ�¼��Ŀ
	SRoleLevelAttr      levels[1];  // ��ɫ�ȼ�������Ϣ
};

// ��ɫս����������Ϣ��ѯ���
struct SGetRoleCPRes
{
	SGetRankInfoResult  result;     // ���в�ѯ���ͺͼ�¼��Ŀ
	SRoleCPAttr         cps[1];     // ��ɫս����������Ϣ
};

// ��ɫ����������Ϣ��ѯ���
struct SGetRoleInstRes
{
	SGetRankInfoResult  result;     // ���в�ѯ���ͺͼ�¼��Ŀ
	SRoleInstAttr       insts[1];   // ��ɫ����������Ϣ
};

// ��ɫ�ɾ͵���������ѯ���
struct SGetRolePointRes
{
	SGetRankInfoResult  result;     // ���в�ѯ���ͺͼ�¼��Ŀ
	SRolePointAttr      points[1];  // ��ɫ�ɾ͵���������Ϣ
};

// ��ɫ����ģʽ������ѯ���
struct SRoleCompetitiveRank
{
	SGetRankInfoResult  result;     // ���в�ѯ���ͺͼ�¼��Ŀ
	SRoleCompAttr       points[1];  // ��ɫ����ģʽ������Ϣ
};

// ��ɫ����������Ϣ�Ĳ�ѯ���
struct SGetRoleAllRankRes
{
	BYTE    querytype;              // ���в�ѯ����
	CHAR    data[1];                // ��ɫ����������Ϣ
};

//ͳһд��־�Ļ�����Ϣ
struct SGSLogBase
{
	BYTE    level;	        //��־����(�μ�roselogex.h�Ķ���)
	BYTE    module;	        //������־�ķ�����(�μ�roselogex.h�Ķ���)
	LTMSEL  time;           //д��־��ʱ��

	void hton()
	{
		//time = xs_hton(time);
	}

	void ntoh()
	{
		//time = xs_ntoh(time);
	}
};

//ͳһд��־����־ͷ
struct SGSLogHead
{
	UINT16      cmdcode;    //������(RANK_SEND_LOG_INFO)
	SGSLogBase  base;
	void hton()
	{
		cmdcode = htons(cmdcode);
		//base.hton();
	}

	void ntoh()
	{
		cmdcode = ntohs(cmdcode);
		//base.ntoh();
	}
};

#pragma pack(pop)

#endif