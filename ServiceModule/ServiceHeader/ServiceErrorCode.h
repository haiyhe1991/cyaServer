#ifndef __SERVICE_ERROR_CODE_H__
#define __SERVICE_ERROR_CODE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/************************************************************************
DBServer(���ݷ�����)�����붨��
*************************************************************************/
#define DBS_OK                  0       //�����ɹ�
#define DBS_ERROR               1       //δ֪����

//�ͻ��˴�����
#define DBC_INVALID_PARAM       1001    //��������
#define DBC_CONNECT_TIMEOUT     1002    //�������ݷ�������ʱ
#define DBC_SERVER_BROKEN       1003    //���ݷ��������ӶϿ�
#define DBC_RECV_TIMEOUT        1004    //�������ݳ�ʱ
#define DBC_INVALID_RESPONSE    1005    //�յ�����ʶ������ݸ�ʽ
#define DBC_ALLOC_FAILED        1006    //�����ڴ�ʧ��

//����˴�����
#define DBS_ERR_PARAM           2000    /// ��������
#define DBS_ERR_COMMAND         2001    /// ��֧�ֵ�����
#define DBS_ERR_SQL             2002    /// sql���ִ��ʧ��
#define DBS_ERR_TABLE           2003    /// Table������
#define DBS_ERR_KEY             2004    /// Key������
#define DBS_ERR_FIELD           2005    /// Field������
#define DBS_ERR_BUSY            2006    /// mysql��æ

/**************************************************************************
GWServer(���ط�����)�����붨��
***************************************************************************/

//�ͻ��˴�����
#define GWS_OK                  0       //�����ɹ�
#define GWS_ERROR               1       //δ֪����
#define GWC_INVALID_PARAM       3001    //��������
#define GWC_CONNECT_TIMEOUT     3002    //���ӳ�ʱ
#define GWC_SERVER_BROKEN       3003    //����������
#define GWC_RECV_TIMEOUT        3004    //�������ݳ�ʱ
#define GWC_SEND_TIMEOUT        3005    //�������ݳ�ʱ
#define GWC_INVALID_RESPONSE    3006    //�յ�����ʶ������ݸ�ʽ
#define GWC_ALLOC_FAILED        3007    //�����ڴ�ʧ��

//����˴�����
#define GWS_LINKER_OFFLINE      4001    //�Զ˲�����
#define GWS_UNKNOWN_SERVERID    4002    //����ʶ��ķ��������
#define GWS_UNKNOWN_PACKET      4003    //����ʶ��İ�����
#define GWS_SESSIONID_IS_EXIST  4004    //�Ựid�Ѵ���
#define GWS_SERVER_NOT_OPEN     4005    //������δ����


/**************************************************************************
LinkServer(Link������)�����붨��
***************************************************************************/

/// ������������
#define LINK_OK                 0       //�����ɹ�
#define LINK_ERROR              1       //δ֪����
#define LINK_SERVER_TYPE        5001    //����ķ�����������
#define LINK_CMD_CODE           5002    //���������
#define LINK_USER_ID            5003    //�û��ظ�
#define LINK_LOGIN_GWS          5004    //��¼����ʧ��
#define LINK_START_SERVICE      5005    //��������ʧ��
#define LINK_CONFIG_INIT        5006    //��ʼ��������Ϣʧ��
#define LINK_ACCOUNT_TOKEN      5007    //�˻�û����֤
#define LINK_TOKEN_INVALID      5008    //�˻�Token��Ч�򲻴���
#define LINK_PAKCET_SIZE        5009    //�û����ݰ�����
#define LINK_CONNECT_LS         5010    //������֤������ʧ��
#define LINK_LOGIN_LS           5011    //��¼��֤������ʧ��
#define LINK_SOCK_READ          5012    //SOCKET������
#define LINK_SOCK_WRITE         5013    //SOCKETд����
#define LINK_MEMERY             5014    //�����ڴ�ʧ��
#define LINK_START              5015    //����ʧ��
#define LINK_STOP               5016    //ֹͣʧ��
#define LINK_STATUS             5017    //��ȡ������״̬ʧ��
#define LINK_ROLE_ID            5018    //û�н����ɫ
#define LINK_USER_INVALID       5019    //��Ч���û�
#define LINK_ROLE_INVALID       5020    //��Ч�Ľ�ɫ

/**************************************************************************
ChatServer(���������)�����붨��
***************************************************************************/
#define CHAT_OK                         0       //�ɹ�
#define CHAT_ERROR                      1       //δ֪����
#define CHAT_GWS_LOGIN_FAILED           6001    //��¼���ط�����ʧ��
#define CHAT_DBS_LOGIN_FAILED           6002    //��¼���ݷ�����ʧ��
#define CHAT_ID_EXIST_IN_CHANNEL        6003    //id��Ƶ�����Ѵ���
#define CHAT_ID_NOT_EXIST_IN_CHANNEL    6004    //id��Ƶ���ڲ�����
#define CHAT_CHANNEL_MEMBERS_EMPTY      6005    //��Ƶ����ԱΪ��
#define CHAT_CHANNEL_NOT_ALLOW_EL       6006    //��Ƶ�����Ͳ����������뿪
#define CHAT_INVALID_CMD_PACKET         6007    //�Ƿ����������ݰ�
#define CHAT_UNKNOWN_CMD_CODE           6008    //δ֪��������


/**************************************************************************
RankServer(���з�����)�����붨��
***************************************************************************/
#define RANK_OK                         0       //�ɹ�
#define RANK_ERROR                      1       //δ֪����
#define RANK_GWS_LOGIN_FAILED           6101    //��¼���ط�����ʧ��
#define RANK_DBS_LOGIN_FAILED           6102    //��¼���ݷ�����ʧ��
#define RANK_DBS_READ_FAILED            6103    //��ȡ���ݿ�ʧ��
#define RANK_INVALID_CMD_PACKET         6104    //�Ƿ����������ݰ�
#define RANK_UNKNOWN_CMD_CODE           6105    //δ֪��������
#define RANK_UNKNOWN_QUERY_TYPE         6106    //δ֪�Ĳ�ѯ����
#define RANK_QUERY_NO_MORE_DATA         6107    //��ѯ�ѵ�ĩβ

/**************************************************************************
LicenceServer(��֤������)�����붨��
***************************************************************************/
#define LCS_OK                          0       //�ɹ�
#define LCS_ERROR                       1       //δ֪����
#define LCS_DBS_LOGIN_FAILED            7001    //��¼���ݷ�����ʧ��
#define LCS_INVALID_TOKEN               7002    //�Ƿ�������
#define LCS_INVALID_CMD_CODE            7003    //����ʶ���������
#define LCS_LINKER_LOGIN_AGAIN          7004    //link�������ظ���½
#define LCS_NOT_EXIST_PARTITION         7005    //�����ڵķ���
#define LCS_LINKER_NOT_LOGIN            7006    //��link������δ��½
#define LCS_MAN_SERVER_LOGIN_AGAIN      7007    //�ⲿ����������ظ���¼
#define LCS_NOT_EXIST_USER              7008    //�����ڵ��û�
#define LCS_USER_LOGIN_AGAIN            7009    //�û��ظ���¼
#define LCS_USER_NOT_LOGIN              7010    //�û�δ��¼
#define LCS_READ_DBS_FAILED             7011    //��ȡ���ݿ�ʧ��
#define LCS_ACCOUNT_PSW_WRONG           7012    //�˺��������
#define LCS_INVALID_SESSION_PACKET      7013    //�Ƿ��Ự���ݰ�
#define LCS_PARTITION_TABLE_EMPTY       7014    //������Ϣ��Ϊ��
#define LCS_ERROR_LOGIC_CALL            7015    //������߼�����
#define LCS_EXISTED_PARTITION           7016    //�Ѿ����ڵķ���
#define LCS_INVALID_CMD_PACKET          7017    //�Ƿ����������ݰ�
#define LCS_OVER_MAX_USER_PAYLOAD       7018    //��������û�����
#define LCS_NOT_EXIST_DBS_ID            7019    //�����ڵ����ݷ�����id
#define LCS_EXIST_ACCOUNT_NAME			7020	//�Ѿ����ڵ��˺���

/**************************************************************************
MLogicServer(���߼�������)�����붨��
***************************************************************************/
#define MLS_OK								0       //�ɹ�
#define MLS_ERROR							1       //δ֪����
#define MLS_READ_DBS_FAILED					8001    //��ȡ���ݿ�ʧ��
#define MLS_ROLE_NOT_EXIST					8002    //��ɫδ������Ϸ
#define MLS_ROLE_IS_EXISTED					8003    //��ɫ�Ѿ�����
#define MLS_RESOURCE_NOT_ENOUGH				8004    //ϵͳ��Դ����
#define MLS_FRIEND_IS_EXISTED				8005    //�����Ѵ���
#define MLS_FRIEND_NOT_EXIST				8006    //���Ѳ�����
#define MLS_DEL_FRIEND_FAILED				8007    //����ʧ��
#define MLS_ID_NOT_EXIST					8008    //��id������
#define MLS_UNKNOWN_CMD_CODE				8009    //����ʶ���������
#define MLS_ROLE_NICK_IS_EXISTED			8010    //��ɫ�ǳ��Ѵ���
#define MLS_SCRIPT_EXE_FAILED				8011    //�ű�ִ��ʧ��
#define MLS_SCRIPT_RETVAL_INVALID			8012    //�ű�����ֵ���Ϸ�
#define MLS_EMAIL_NOT_EXIST					8013    //���ʼ�������
#define MLS_ACCOUNT_NOT_EXIST				8014    //�˺Ų�����
#define MLS_BACKPACK_IS_FULL				8015    //��������
#define MLS_INVALID_PACKET					8016    //�Ƿ������ݰ�
#define MLS_ROLE_IS_ONLINE					8017    //��ɫ��������״̬���ܱ�ɾ��
#define MLS_GET_ROLE_ID_FAILED				8018    //��ȡ��ɫidʧ��
#define MLS_ROLE_TYPE_INVALID				8019    //δ֪�Ľ�ɫ����
#define MLS_GEM_SLICE_NO_INLAID				8020    //��ʯƬ������Ƕ
#define MLS_GEM_HOLE_IS_INLAID				8021    //��ʯ������Ƕ��ʯ
#define MLS_PROPERTY_NOT_EXIST				8022    //��Ʒ������
#define MLS_GEM_NOT_INLAID					8023    //��ʯδ��Ƕ
#define MLS_GUILD_NOT_EXIST					8024    //���᲻����
#define MLS_OVER_MAX_ROLES_THRESHOLD		8025    //��ɫ�����������ֵ
#define MLS_OVER_ROLES_MAX_LEVEL			8026    //������ɫ�����ȼ�
#define MLS_SKILL_ID_NOT_EXIST				8027    //�ü��ܲ�����
#define MLS_OVER_MAX_SKILL_LEVEL			8028    //�����ü������ȼ�
#define MLS_VIT_NOT_ENOUGH					8029    //��������
#define MLS_GOLD_NOT_ENOUGH					8030    //��Ҳ���
#define MLS_NOT_EXIST_INST					8031    //�����ڵĸ���
#define MLS_LOWER_INST_LIMIT_LEVEL			8032    //���ڸ������ƽ�ɫ�ȼ�
#define MLS_PROP_LV_HIGHER_THAN_ROLE		8033    //��Ʒ�ȼ����ڽ�ɫ�ȼ�
#define MLS_OVER_MAX_INST_ENTER_TIMES		8034    //������ǰ�������������
#define MLS_NOT_ENTER_INST					8035    //δ����ĸ���
#define MLS_HP_NOT_ENOUGH					8036    //HP����
#define MLS_MP_NOT_ENOUGH					8037    //MP����
#define MLS_ROLE_NOT_HAS_SKILL				8038    //��ɫû�и��ּ���
#define MLS_NOT_EXIST_JOB					8039    //�����ڵ�ְҵ
#define MLS_LOWER_SKILL_LIMIT_LEVEL			8040    //���ڼ������ƽ�ɫ�ȼ�
#define MLS_NOT_EXIST_TASK					8041    //�����ڵ�����
#define MLS_LOWER_TASK_LIMIT_LEVEL			8042    //�����������ƽ�ɫ�ȼ�
#define MLS_OVER_MAX_TASK_NUM				8043    //������ǰ��������������
#define MLS_EXIST_TASK_QUEUE				8044    //�������Ѵ��ڽ������������
#define MLS_FIN_TASK_FAILED					8045    //�������ʧ��
#define MLS_FIN_INST_FAILED					8046    //�������ʧ��
#define MLS_MAX_STRENGTHEN_LEVEL			8047    //�ﵽ���ǿ���ȼ�
#define MLS_MATERIAL_NOT_ENOUGH				8048    //���ϲ���
#define MLS_STRENGTHEN_FAILED				8049    //ǿ��ʧ��
#define MLS_MAX_STAR_LEVEL					8050    //װ���ﵽ����Ǽ�
#define MLS_STAR_LIMIT_LEVEL				8051    //�������ǽ�ɫ���Ƶȼ�
#define MLS_PROP_NOT_ALLOW_SELL				8052    //��Ʒ���������
#define MLS_NOT_ALLOW_COMPOSE				8053    //���ܺϳ�
#define MLS_COMPOSE_FAILED					8054    //�ϳ�ʧ��
#define MLS_ALREADY_WEAR_EQUIP				8055    //��λ���Ѵ�����װ��
#define MLS_WEAR_POS_NOT_MATCH				8056    //����λ�ò�ƥ��
//by hxw 20150901
//#define MLS_NOT_ALLOW_INHERIT				8057    //������̳�
#define MLS_WEAR_LV_LIMIT					8057	//���ڴ���װ���ȼ����� 
//end
#define MLS_GEM_HOLE_NOT_OPEN				8058    //��ʯ��δ��
#define MLS_GEM_AND_HOLE_NOT_MATCH			8059    //��ʯ�ͱ�ʯ�ײ�ƥ��
#define MLS_GEM_HOLE_INVALID				8060    //��ʯ�׺Ų��Ϸ�
#define MLS_GEM_MATERIAL_NOT_MATCH			8061    //��ʯ�ϳɲ��ϲ�ƥ��
#define MLS_NO_COMPOS_QUALITY				8062    //װ���ϳ�Ʒ�ʲ�����
#define MLS_BOX_NOT_EXIST					8063    //���䲻����
#define MLS_NICK_NOT_ALLOW_EMPTY			8064    //�ǳƲ�����Ϊ��
#define MLS_NO_PROP_SELL_RCD				8065    //����Ʒ���ۼ�¼
#define MLS_PROP_ATTR_IS_FULL				8066    //��Ʒ��������
#define MLS_GOODS_NOT_EXIST					8067    //��Ʒ������
#define MLS_CASH_NOT_ENOUGH					8068    //���Ҳ���
#define MLS_OVER_MAX_DRESS_LIMIT			8069    //����ӵ�����ʱװ��������
#define MLS_DRESS_NOT_EXIST					8070    //ʱװ������
#define MLS_DRESS_OUT_DATE					8071    //ʱװ����
#define MLS_JOB_DRESS_NOT_MATCH				8072    //ְҵʱװ��ƥ��
#define MLS_VIP_LV_NOT_ENOUGH				8073    //VIP�ȼ�����
#define MLS_INVALID_CHASH_ID				8074    //�Ƿ��ĳ�ֵid
#define MLS_EQUIP_STRENGTH_LV0				8075    //װ��ǿ���ȼ�Ϊ0��������
#define MLS_EQUIP_STAR_LV0					8076    //װ���Ǽ�Ϊ0��������
#define MLS_TITLE_NOT_EXIST					8077    //�ƺŲ�����
#define MLS_NOT_OPEN_RAND_INST				8078    //δ�������������
#define MLS_RAND_INST_FULL					8079    //���������������
#define MLS_NOT_EXIST_SUIT					8080    //�����ڵ���װ
#define MLS_INVALID_REVIVE					8081    //�Ƿ��ĸ���
#define MLS_OVER_MAX_REVIVE_TIMES			8082    //������󸴻����
#define MLS_NOT_EXIST_EXCHANGE_ID			8083    //�����ڵĽ�Ҷһ�id
#define MLS_OVER_DAY_EXCHANGE_TIMES			8084    //�����������һ�����
#define MLS_OVER_MAX_FRIENDS				8085    //������������������
#define MLS_NOT_FIND_PLAYER					8086    //δ���ҵ������Ϣ
#define MLS_ALREADY_RECEIVED_VIP			8087    //�Ѿ���ȡ��VIP����
#define MLS_NOT_EXIST_MONSTER				8088    //�����ڵĹ�
#define MLS_FINISH_TASK_FAILED				8089    //�������ʧ��
#define MLS_ALREADY_FINISHED_HELP			8090    //����ɵ���������
#define MLS_HAS_INST_DROP_EMAIL				8091    //�и��������ʼ�δ��ȡ
#define MLS_BATTLE_AREA_NOT_EXIST			8092    //ս�����򲻴���
#define MLS_NOT_EXIST_COMPOSE_ID			8093    //�����ڵĺϳ�ID
#define MLS_ROLE_LV_OUT_RANGE				8094    //��ɫ�ȼ�������Χ
#define MLS_DATETIME_OUT_RANGE				8095    //���ڳ�����Χ
#define MLS_NOT_HAS_MAGIC					8096    //��ħ����
#define MLS_UNKNOWN_EQUIP_POS				8097    //δ֪��װ��λ
#define MLS_NOT_EXIST_CHAPTER				8098    //�����ڵ��½�
#define MLS_ALREADY_SIGN_IN					8099    //�����Ѿ�ǩ��
#define MLS_INVALID_SIGN_IN					8100    //�Ƿ���ǩ��
#define MLS_NOT_EXIST_ACTIVITY				8101    //�����ڵĻ
#define MLS_ACTIVITY_OUT_DATE				8102    //�����
#define MLS_ALREADY_RECEIVED_ACTIVITY		8103    //�Ѿ���ȡ�Ļ
#define MLS_INVALID_CASH_CODE				8104	//�Ƿ��Ķһ���
#define MLS_CASH_CODE_EXPIRED				8105	//�һ������
#define MLS_INVALID_UNLOCK_ID				8106	//�Ƿ��Ľ���id
#define MLS_INVALID_GIFTBAG_ID				8107	//�Ƿ������id
#define MLS_GENERATE_ORDER_FAILED			8108	//���ɶ���ʧ��
#define MLS_GET_ORDER_RESULT_FAILED			8109	//��ȡ�������ʧ��
#define MLS_INVALID_ORDER					8110	//��Ч�Ķ���
#define MLS_ORDER_NOT_CONFIRMED				8111	//��������δȷ��״̬
#define MLS_ALREADY_GET_ORDER				8112	//�Ѿ���ȡ���Ķ���
#define MLS_INST_FAILED_TIMEOVER			8113	//�����������ʱ��
#define MLS_INST_FAILED_DEAD				8114	//��ɫ����
#define MLS_INST_FAILED_EXIT				8115	//��ɫ��;�˳�
#define MLS_INST_FAILED_STONE_BROKEN		8116	//ʯ������
#define MLS_RECEIVED_BIG_REWARD				8117	//���Ѿ���ȡ
#define MLS_NOT_FIN_EXTRACT					8118	//δ��ɳ齱
#define MLS_ALREADY_FIN_EXTRACT				8119	//�Ѿ���ɳ齱
#define MLS_INVALID_ACHIEVEMENT_TYPE		8120	//��Ч�ɾ�����
#define MLS_ACHIEVEMENT_NOT_REACHED			8121	//�ɾ�δ���
#define MLS_ACHIEVEMENT_HAVE_RECEIVED		8122	//�ɾ�����ȡ
#define MLS_TODAY_HAS_SIGNED				8123	//������ǩ��
#define MLS_INVALID_CHAINSOUL_POS			8124	//��Ч�����겿��
#define MLS_CHAINSOUL_FRAGMENT_NOT_ENOUGH	8125	//������Ƭ����
#define MLS_UPGRADE_CHAINSOUL_POS_FAIL		8126	//�������겿��ʧ��

//add by hxw 20151014
#define MLS_QLV_RANK_REWARD_NULL			8127	//�޵ȼ����н�����ȡ
#define MLS_QINUM_RANK_REWARD_NULL			8128	//�޸�����ս���н�����ȡ
#define MLS_QINUM_INFO_RANK_REWARD_FAIL		8129	//��ѯ������ս����������Ϣ����
#define MLS_QCP_REWARD_FAIL					8130	//��ѯǿ�߽���ʧ��
#define MLS_GET_CP_REWARD_FAIL				8131	//��ȡǿ�߽���ʧ��
#define MLS_GET_RANK_REWARD_CONFIG_FAIL		8132	//��ȡͨ�����н�����Ʒ���ô���
//end

#define MLS_NOT_OPEN_WORLD_BOSS				8133	//����Bossδ����
#define MLS_NOT_OPEN_GUARD_ICON				8134	//����ʥ��δ����
#define MLS_INVALID_LADDER_RANK				8135	//��Ч����������
#define MLS_LADDER_RANK_LOCKED				8136	//���������ѱ�����
#define MLS_DONT_CHALLENGE_SELF				8137	//������ս�Լ�
#define MLS_FINISH_CHALLENGE_LADDER_FAIL	8138	//�����ս����ʧ��
#define MLS_FINISH_CHALLENGE_LADDER_TIMEOUT 8139	//�����ս���ݳ�ʱ
#define MLS_LADDER_CHALLENGE_COOLING		8140	//������ս������ȴ��
#define MLS_LADDER_CHALLENGE_UPPER_LIMIT	8141	//������ս��������
#define MLS_BACKPACK_UPPER_LIMIT			8142	//������������
#define MLS_WAREHOUSE_UPPER_LIMIT			8143	//�ֿ��������
#define MLS_EXIST_SAME_JOB					8144	//������ͬ��ְҵ
#define MLS_NOT_HAS_ACTIVITY_INST_TYPE		8145	//��ӵ�еĻ��������
#define MLS_ENTER_COMPMODE_UPPER			8146	//����ģʽ��������
#define MLS_BUY_ACHIEVE_UPPER				8147	//����ﵽ����
#define MLS_VIP_BUY_ACHIEVE_UPPER			8148	//VIP����ﵽ����
#define MLS_ENTER_ONE_VS_ONE_UPPER			8149	//1v1ģʽ��������
#define MLS_PROPERTY_NOT_ENOUGH				8150	//��Ʒ��������

//add by hxw 20151020
#define MLS_QONLINE_REWARD_FAIL				8201	//��ȡ���߽�����Ϣ����
#define MLS_GONLINE_REWARD_FAIL				8202	//��ȡ���߽�������
#define MLS_REWARD_ISGET					8203	//�Ѿ���ȡ���ý���
//end
//add by hxw 20151028
#define MLS_QLV_INFO_RANK_REWARD_FAIL		8204	//��ѯ�ȼ����а���Ϣ����
//end
//add by hxw 20151029
#define MLS_REWARD_CANNOT_GET				8205	//δ�ﵽ������ȡ����
//end
//add by hxw 20151117
#define MLS_CASH_SAVE_TYPE_ERROR			8206	//�����ļ��еĴ��Ҵ洢���ʹ���
//end

/**************************************************************************
ExtServer(�û����������)�����붨��
***************************************************************************/
#define EXT_OK							0       //�ɹ�
#define EXT_ERROR						1       //δ֪����
#define EXT_CONFIG_INIT					9001    //��ʼ�������ļ�ʧ��
#define EXT_ACCOUNT						9002    //�˻���Ϣ����
#define EXT_COMAND						9003    //����ʧ��
#define EXT_LOGIN_REPEATED				9004    //�û��ظ���¼
#define EXT_NOT_LOGIN					9005    //�û�δ��¼

/*******************************************************************************
***********************************���������������*****************************
*******************************************<br class="Apple-interchange-newline">*************************************/
#define		GM_OK										0			//OK
#define		GM_ERROR									1			//��������
#define		GM_INVALID_COMMAND							10000		//���ܺϷ���������
#define		GM_INVALID_SOCKET_CONN_LINKER				10001		//����Linker Socket������
#define		GM_READ_LINKER_DATA_TIMEOUT					10002		//����linker����ʧ��
#define		GM_WRITE_LINKER_DATA_TIMEOUT				10003		//����linker����ʧ��
#define		GM_PARSE_KEY_FAILED							10004		//����Linker���͵ļ���keyʧ��
#define		GM_QUERY_ACCOUNT_LK_FAILED					10005		//��ѯ�ʺ�����ʧ��
#define		GM_QUERY_DEVICE_LK_FAILED					10006		//��ѯ�豸����ʧ��
#define		GM_QUERY_ACTOR_LK_FAILED					10007		//��ѯ��ɫ����ʧ��
#define		GM_INVALID_PARAM							10008		//��Ч�Ĳ���
#define		GM_UNKNOWN_LKTYPE							10009		//����ʶ�����������
#define		GM_LK_STORE_DB_FALIED						10010		//��������д�����ݿ�ʧ��
#define		GM_ONLINE_STORE_DB_FALIED					10011		//��������д�����ݿ�ʧ��
#define		GM_AVERAGE_ONLINE_FALIED					10012		//��ȡƽ����������ʧ��
#define		GM_INVALID_LKTIME_TYPE						10013		//��Ч������ʱ������
#define		GM_AVERAGE_ONLINE_TIME_FAILED				10014		//��ȡƽ������ʱ��ʧ��
#define		GM_NOT_EXISTS_SESSION						10015		//�����ڵĻỰ
#define		GM_USER_LOGIN_FAILED						10016		//��¼��֤ʧ��
#define		GM_ACCOUNT_INVALID							10017		//��Ч���ʺ���Ϣ
#define		GM_GET_ACTIVE_DEVICE_FAILED					10018		//��ȡ�����豸��ʧ��
#define		GM_GET_ACCOUNT_CREATE_FAILED				10019		//��ȡ�ʺŴ�����ʧ��
#define		GM_GET_LOGIN_USER_FAILED					10020		//��ȡ��½�û���ʧ��
#define		GM_GET_ADD_NEW_FAILED						10021		//��ȡ�����û���ʧ��
#define		GM_GET_ACTIVE_ACTOR_FAILED					10022		//��ȡ��Ծ����ʧ��
#define		GM_GET_ADDUP_NEW_FAILED						10023		//��ȡ�ۼ�����ʧ��
#define		GM_GET_AVERAGE_TIME_FAILED					10024		//��ȡƽ������ʱ��ʧ��
#define		GM_GET_AVERAGE_PLAYGAME_FAILED				10025		//��ȡƽ����Ϸ����ʧ��
#define		GM_GET_ONLINE_FAILED						10026		//��ȡʵʱ����ʧ��;
#define		GM_GET_MAX_ONLINE_FAILED					10027		//��ȡ�����������ʧ��
#define		GM_GET_AVERAGE_ONLINE_FAILED				10028		//��ȡƽ����������ʧ��
#define		GM_GET_PAYMENT_ACTOR_FAILED					10029		//��ȡ�����û���ʧ��
#define		GM_GET_PAYMENT_COUNT_FAILED					10030		//��ȡ�����ô���
#define		GM_GET_FIRST_RECHARGE_COUNT_FAILED			10031		//��ȡ�����û��׳����ʧ��
#define		GM_GET_ADDUP_NEW_PAYMENT_FAILED				10032		//��ȡ�ۼ����������û���ʧ��
#define		GM_GET_ADD_NEW_PAYMENT_MONEY_FAILED			10033		//��ȡ�������ѽ��ʧ��
#define		GM_GET_PAYMENT_MONEY_FAILED					10034		//��ȡ���ѽ��ʧ��
#define		GM_TOTAL_PAYMENT_FAILED						10035		//��ȡ�ۼƸ��ѽ��ʧ��
#define		GM_ACTIVATED_DEVICE_FAILED					10036		//��ȡ�����豸ʧ��
#define		GM_COST_TOTAL_FAILED						10037		//��ȡ�����ܽ��ʧ��
#define		GM_SINGLE_ACTOR_COST_TOTAL_FAILED			10038		//��ȡ������ɫ�����ܽ��ʧ��
#define		GM_ACCOUNT_DETAIL_FAILED					10039		//��ȡ�ʺ�����ʧ��
#define		GM_ACTOR_DETAIL_FAILED						10040		//��ȡ��ɫ����ʧ��
#define		GM_CONN_SERVER_FAILED						10041		//���ӷ�����ʧ��
#define     GM_QUERY_NEWSTATISTICS_FAILED				10042	//��ȡ����ͳ������ʧ��;
#define		GM_RECHARGE_STATISTICS_FAILED				10043	//��ȡ��ѯ������ֵ����ʧ��
#define		GM_RECHARGE_STATISTICS_NUM					10044	//��ȡ��ֵ����ͳ��ʧ��;
#define		GM_QUERY_ONLINENUM_STATISTICS_FAILED		10045	//��ȡ��������ͳ��ʧ��;
#define		GM_QUERY_ONLINENUM_NUM_FAILED				10046	//��ȡ��������ʧ��;
#define		GM_QUERY_LEVEL_DISTRIBUTION_FAILED			10047	//��ȡ�ȼ��ֲ�ʧ��;
#define     GM_QUERY_TIME								10048	//ʱ�����;

#endif