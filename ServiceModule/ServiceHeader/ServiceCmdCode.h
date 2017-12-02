#ifndef __SERVICE_CMD_CODE_H__
#define __SERVICE_CMD_CODE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#define LINK_OTHER_LOGIC_SERVER(cmdCode)    ((0x6AA4) <= (cmdCode) && (0x6B6B) >= (cmdCode))

/************************************************************************
0x61A9------����������������룩
*************************************************************************/
//�����û�����
#define MANS_USER_LOGIN					0x61A9	//25001, ��¼��¼
#define MANS_USER_LOGOUT				0x61AA	//25002, �û��˳�

//��֤����������
#define LCS_QUERY_ACCOUNT_PARTITION		0x61B3	//25011, ��ѯ����˻�����
#define LCS_QUERY_PLAYER_ACCOUNT		0x61B4	//25012, ��ѯ����˻�
#define LCS_QUERY_GAME_PARTITION		0x61B5	//25013, ��ѯ��Ϸ����
#define LCS_ADD_PARTITION				0x61B6	//25014, ���ӷ���
#define LCS_DEL_PARTITION				0x61B7	//25015, ɾ������
#define LCS_MODIFY_PARTITION			0x61B8	//25016, �޸ķ���
#define LCS_SET_RECOMMEND_PARTITION		0x61B9	//25017, �����Ƽ�����
#define LCS_RELOAD_PARTITION			0x61BA	//25018, �����������
#define LCS_START_LINKER_SERVE			0x61BB	//25019, ����link����
#define LCS_STOP_LINKER_SERVE			0x61BC	//25020, ֹͣlink����
#define LCS_START_PARTITION_SERVE		0x61BD	//25021, ��������
#define LCS_STOP_PARTITION_SERVE		0x61BE	//25022, ֹͣ����


//��Ϸ�߼�����������
#define LGS_QUERY_PLAYER_LIST			0x61E5	//25061, ��ѯ����б�
#define LGS_SEND_PLAYER_EMAIL			0x61E6	//25062, ���û������ʼ�

/************************************************************************
0x6AA4------0x6B6B�����з��������룩
*************************************************************************/
#define RANK_QUERY_ROLE_INFO		    0x6AA4	//27300, ��ѯ��ҵ�������Ϣ
#define RANK_QUERY_ROLE_RANK		    0x6AA5	//27301, ��ѯ��ǰ��ҵ�ĳ��������Ϣ
#define RANK_SEND_LOG_INFO		        0x6AA6	//27302, ����log��Ϣ�����з�����

/************************************************************************
0x6B6C------0x7530��������������룩
*************************************************************************/
#define CHAT_JOIN_CHANNEL			    0x6B6C	//27500,����Ƶ��
#define CHAT_LEAVE_CHANNEL			    0x6B6D	//27501,�뿪Ƶ��
#define CHAT_SEND_MESSAGE			    0x6B6E	//27502,������Ϣ
#define CHAT_RECV_MESSAGE			    0x6B6F	//27503,�յ���Ϣ
#define CHAT_USER_EXIT				    0x6B70	//27504,�û��˳�
#define CHAT_USER_OFFLINE_NOTIFY	    0x6B71	//27505,�û�������֪ͨ


/************************************************************************
0x7531------����֤���������룩
*************************************************************************/
#define LICENCE_LINKER_LOGIN					0x7531	//30001,link��������¼
// #define LICENCE_START_PARTITION_SERVE		0x7532	//30002,��������
// #define LICENCE_STOP_PARTITION_SERVE			0x7533	//30003,ֹͣ����
#define LICENCE_LINKER_GET_TOKEN				0x7534	//30004,��link��ȡtoken
#define LICENCE_LINKER_QUERY_PARTITION_STATUS	0x7535	//30005,link��������ѯ��ǰ���ڷ���״̬
#define LICENCE_LINKER_REPORT_CONNECTIONS		0x7536	//30006,link�������ϱ���ǰ������
#define LICENCE_LINKER_ROLE_CHANGE				0x7537	//30007,link�������㱨��ɫ������ɾ��

#define LICENCE_USER_LOGIN						0x7538	//30008,�û���¼
#define LICENCE_USER_QUERY_PARTITION			0x7539	//30009,�û���ѯ������Ϣ
#define LICENCE_USER_ENTER_PARTITION			0x753A	//30010,�û������������
#define LICENCE_USER_REGIST						0x753B	//30011,�û�ע��

// #define LICENCE_PARTITION_ADD				0x753C	//30012,���ӷ���
// #define LICENCE_PARTITION_DEL				0x753D	//30013,ɾ������
// #define LICENCE_PARTITION_MODIFY				0x753E	//30014,�޸ķ���
// #define LICENCE_PARTITION_QUERY_INFO			0x753F	//30015,��ѯ����
// #define LICENCE_PARTITION_SET_RECOMMEND		0x7540	//30016,�����Ƽ�����
// #define LICENCE_PARTITION_RELOAD				0x7541	//30017,�����������

// #define LICENCE_START_LINKER_SERVE			0x7542	//30018,����link����
// #define LICENCE_STOP_LINKER_SERVE			0x7543	//30019,ֹͣlink����

#define LICENCE_LINKER_USER_CHECK				0x7544	//30020,linker֪ͨ��֤�������û���֤ͨ��
#define LICENCE_NOTIFY_USER_OFFLINE				0x7545	//30021,֪ͨlinkerҪ���ߵ��û�
#define LICENCE_QUERY_USER_LOGIN				0x7546	//30022,��ѯ�û���½��Ϣ
#define LICENCE_GET_REPORT_TIMER				0x7547	//30023,��ȡ�ϱ�linker������ʱ����(��λ:��)

#define LICENCE_QUERY_UNLOCK_JOBS				0x7548	//30024,��ѯ����ְҵ
#define LICENCE_LINKER_QUERY_UNLOCK_JOBS		0x7549	//30025,Linker��ѯ����ְҵ


/************************************************************************
0x4E20------0x55F0��Linker���������룩	///
*************************************************************************/
#define LINKER_LOGIN							0x4E20	// 20000����½Linker������
#define LINKER_LOGOUT							0x4E21	// 20001���ǳ�Linker������
#define LINKER_HEARTBEAT						0x4E22	// 20002, ����
#define LINKER_GET_ONLINE_CONNECT_NUM			0x4E23	// 20003, ��ȡ��ǰLinker��������

/************************************************************************
0x2710------0x4E1F�����߼����������룩	///
*************************************************************************/
#define MLS_QUERY_ROLES							0x2710	//10000, ��ѯ��ɫ��Ϣ
#define MLS_ENTER_ROLE							0x2711	//10001, �����ɫ
#define MLS_QUERY_BACKPACK						0x2712	//10002, ��ѯ����
#define MLS_QUERY_ACHIEVEMENT					0x2713	//10003, ��ѯ�ɾ�
#define MLS_QUERY_EMAIL_CONTENT					0x2714	//10004, ��ѯ�ʼ�����
#define MLS_CREATE_ROLE							0x2715	//10005, ������ɫ
#define MLS_QUERY_FRIEND						0x2716	//10006, ��ѯ����
#define MLS_ADD_FRIEND							0x2717	//10007, ��Ӻ���
#define MLS_DEL_FRIEND							0x2718	//10008, ɾ������
#define MLS_QUERY_FINISHED_HELP					0x2719	//10009, ��ѯ���������������Ϣ
#define MLS_QUERY_FINISHED_INST					0x271A	//10010, ��ѯ����ɸ�����Ϣ
#define MLS_BUFF_OVER_NOTIFY					0x271B	//10011, buf����֪ͨ
#define MLS_SAVE_GEAR_DESTROY_STATE				0x271C	//10012, ��������ƻ�״̬
#define MLS_SAVE_ROLE_ATTR						0x271D	//10013, �������Ե�
#define MLS_FINISH_ROLE_HELP					0x271E	//10014, �����������
#define MLS_BACKPACK_MOD_NOTIFY					0x271F	//10015, ������������֪ͨ
#define MLS_QUERY_CUR_CASH						0x2720	//10016, ��ѯ��ǰ�һ���
#define MLS_QUERY_ROLE_APPEARANCE				0x2721	//10017, ��ѯ��ɫ������Ϣ
#define MLS_QUERY_ROLE_EXPLICIT					0x2722	//10018, ��ѯ��ɫ��ϸ��Ϣ
#define MLS_QUERY_GIFT_BAG						0x2723	//10019, ��ѯ��Ʒ��
#define MLS_QUERY_ACCEPTED_TASK					0x2724	//10020, ��ѯ�ѽ�������
#define MLS_DELTA_TOKEN							0x2725	//10021, ��ֵ����
#define MLS_DELTA_NOTIFY						0x2726	//10022, ��ֵ֪ͨ
#define MLS_SELL_PROPERTY						0x2727	//10023, ���۵���
#define MLS_FLUSH_CASH							0x2728	//10024, ˢ�¶һ���
#define MLS_DROP_PROPERTY						0x2729	//10025, ��������
#define MLS_CASH_PROPERTY						0x272A	//10026, �һ�����
#define MLS_DROP_TASK							0x272B	//10027, ��������
#define MLS_NEW_EMAIL_NOTIFY					0x272C	//10028, ����������֪ͨ�����ʼ�
#define MLS_PICKUP_INST_BOX						0x272D	//10029, ʰȡ��������
#define MLS_BUY_STORAGE_SPACE					0x272E	//10030, ���򱳰���ֿⴢ��ռ�
#define MLS_ACCEPT_TASK							0x272F	//10031, ��������
#define MLS_ENTER_INST							0x2730	//10032, ���븱��
#define MLS_RECEIVE_REWARD						0x2731	//10033, ��ȡ�ɾͽ���
#define MLS_UPDATE_NOTIFY						0x2732	//10034, ����֪ͨ
#define MLS_USE_PROPERTY						0x2733	//10035, ʹ�õ���
#define MLS_RECEIVE_EMAIL_ATTACHMENT			0x2734	//10036, ��ȡ�ʼ�����
#define MLS_FINISH_INST							0x2735	//10037, ���ĳ����
#define MLS_FETCH_ROBOT							0x2736	//10038, ��ȡ����������
//#define MLS_INLAID_GEM							0x2737	//10039, ��Ƕ��ʯ
//#define MLS_MOD_ACCEPTED_TASK_STATE				0x2738	//10040, �޸��ѽ�������״̬
#define MLS_QUERY_EMAIL_LIST					0x2739	//10041, �ʼ���ѯ�б�
#define MLS_MOD_ROLE_ATTR						0x273A	//10042, �������Ե�
#define MLS_LOAD_EQUIPMENT						0x273B	//10043, ����װ��
#define MLS_UNLOAD_EQUIPMENT					0x273C	//10044, ж��װ��
#define MLS_LEAVE_ROLE							0x273D	//10045, �˳���ɫ
#define MLS_DEL_ROLE							0x273E	//10046, ɾ����ɫ
//#define MLS_REMOVE_GEM							0x273F	//10047, ȡ����ʯ
#define MLS_QUERY_GUILD_NAME					0x2740	//10048, ��ѯ��������
#define MLS_USER_EXIT							0x2741	//10049, �˺��˳�
//#define MLS_QUERY_INST_RANK						0x2742	//10050, ��ѯĳ����������Ϣ
#define MLS_PERSONAL_TIMER_TASK					0x2743	//10051, ���˶�ʱ��������
#define MLS_PERSONAL_TIMER_TASK_FIN				0x2744	//10052, ���˶�ʱ�������
#define MLS_FETCH_INST_AREA_DROPS				0x2745	//10053, ��ȡ��������ֵ���
#define MLS_ROLE_UPGRADE_NOTIFY					0x2746	//10054, ��ɫ����֪ͨ
#define MLS_QUERY_ROLE_SKILLS					0x2747	//10055, ��ѯ��ɫ���м���
#define MLS_SKILL_UPGRADE						0x2748	//10056, ��������
#define MLS_CHANGE_FACTION						0x2749	//10057, �ı���Ӫ
#define MLS_LOAD_SKILL							0x274A	//10058, װ�ؼ���
#define MLS_CANCEL_SKILL						0x274B	//10059, ȡ������
//#define MLS_SET_SKILL_CLIENT_ID					0x274C	//10060, ���ü��ܿͻ���id
#define MLS_UPDATE_TASK_TARGET					0x274D	//10061, �����ѽ�����Ŀ��
#define MLS_COMMIT_TASK_FINISH					0x274E	//10062, �ύ�������
#define MLS_TASK_STATUS_UPDATE_NOTIFY			0x274F	//10063, ����״̬����֪ͨ
#define MLS_QUERY_FIN_TASK_HISTORY				0x2750	//10064, ��ѯ�����������ʷ��¼
#define MLS_GET_DAILY_TASK						0x2751	//10065, ��ȡ�ճ�����
//#define MLS_STRENGTHEN_EQUIPMENT				0x2752	//10066, ǿ��װ��
//#define MLS_EQUIPMENT_RISE_STAR					0x2753	//10067, װ������
#define MLS_EQUIPMENT_COMPOSE					0x2754	//10068, װ���ϳ�
//#define MLS_EQUIPMENT_CLEAR_STAR				0x2755	//10069, װ������
//#define MLS_EQUIPMENT_INHERIT					0x2756	//10070, װ���̳�
//#define MLS_EQUIPMENT_STRENGTHEN_INHERIT		0x2757	//10071, װ��ǿ���̳�
//#define MLS_EQUIPMENT_STRENGTHEN_CLEAR			0x2758	//10072, װ��ǿ������
#define MLS_GEM_COMPOSE							0x2759	//10073, ��ʯ�ϳ�
#define MLS_GEM_DECOMPOSE						0x275A	//10074, ��ʯ�ֽ�
#define MLS_INST_OCCUPY_PUSH_NOTIFY				0x275B	//10075, ����ռ������֪ͨ
#define MLS_PROP_BUY_BACK						0x275C	//10076, ��Ʒ�ع�
#define MLS_QUERY_ROLE_SELL_PROP_RCD			0x275D	//10077, ��ѯ��ɫ������Ʒ��¼
//#define MLS_FRAGMENT_COMPOSE_EQUIP				0x275E	//10078, ��Ƭ�ϳ�װ��
#define MLS_VIT_UPDATE_NOTIFY					0x275F	//10079, ��������֪ͨ
#define MLS_BACKPACK_CLEAN_UP					0x2760	//10080, ��������
#define MLS_GET_ROLE_OCCUPY_INST				0x2761	//10081, ��ȡ��ɫռ�츱��
#define MLS_SET_ROLE_TITLE_ID					0x2762	//10082, ���ý�ɫְҵ�ƺ�id
#define MLS_DEL_EMAIL							0x2763	//10083, ɾ����ɫ�ʼ�
#define MLS_MALL_BUY_GOODS						0x2764	//10084, �̳ǹ�����Ʒ
#define MLS_QUERY_ROLE_DRESS					0x2765	//10085, ��ѯ��ɫӵ��ʱװ
#define MLS_WEAR_DRESS							0x2766	//10086, ����ʱװ
#define MLS_PICKUP_INST_MONSTER_DROP			0x2767	//10087, ʰȡ�����ֵ�����Ʒ
#define MLS_GOLD_UPDATE_NOTIFY					0x2768	//10088, ��ɫ��Ҹ���֪ͨ
#define MLS_TOKEN_UPDATE_NOTIFY					0x2769	//10089, ��ɫ���Ҹ���֪ͨ
#define MLS_EXP_UPDATE_NOTIFY					0x276A	//10090, ��ɫ�������֪ͨ
#define MLS_BACKPACK_UPDATE_NOTIFY				0x276B	//10091, ��ɫ��������֪ͨ
#define MLS_RAND_INST_OPEN_NOTIFY				0x276C	//10092, �����������֪ͨ
#define MLS_RAND_INST_CLOSE_NOTIFY				0x276D	//10093, ��������ر�֪ͨ
#define MLS_GET_PERSONAL_TRANSFER_ENTER			0x276E	//10094, ��ȡ������������������������
#define MLS_QUERY_EQUIP_FRAGMENT				0x276F	//10095, ��ѯװ����Ƭ
//#define MLS_EQUIP_FRAGMENT_COMPOSE				0x2770	//10096, װ����Ƭ�ϳ�
#define MLS_ROLE_REVIVE							0x2771	//10097, ��ɫ����
#define MLS_GET_OPEN_PERSONAL_RAND_INST			0x2772	//10098, ��ȡ�Ѵ򿪵����������
#define MLS_SYS_NOTICE_NOTIFY					0x2773	//10099, ϵͳ����֪ͨ
#define MLS_QUERY_GOLD_EXCHANGE					0x2774	//10100, ��ѯ�����Ҷһ�����
#define MLS_GOLD_EXCHANGE						0x2775	//10101, ��Ҷһ�
#define MLS_FRIEND_APPLY_NOTIFY					0x2776	//10102, ��������֪ͨ
#define MLS_FRIEND_APPLY_CONFIRM				0x2777	//10103, ��������ȷ��
#define MLS_AGREE_FRIEND_APPLY_NOTIFY			0x2778	//10104, ��������ͬ��֪ͨ
#define MLS_QUERY_FRIEND_APPLY					0x2779	//10105, ��ѯ��������
#define MLS_GET_RECOMMEND_FRIEND_LIST			0x277A	//10106, ��ȡ�Ƽ������б�
#define MLS_SEARCH_PLAYER						0x277B	//10107, �������
#define MLS_FRIEND_ONLINE_NOTIFY				0x277C	//10108, ��������֪ͨ
#define MLS_FRIEND_OFFLINE_NOTIFY				0x277D	//10109, ��������֪ͨ
#define MLS_REFUSE_FRIEND_APPLY_NOTIFY			0x277E	//10110, �ܾ���������֪ͨ
#define MLS_FRIEND_DEL_NOTIFY					0x277F	//10111, ����ɾ��֪ͨ
#define MLS_NOT_RECEIVE_VIP_REWARD				0x2780	//10112, δ��ȡ������VIP�ȼ�
#define MLS_RECEIVE_VIP_REWARD					0x2781	//10113, ��ȡVIP����
#define MLS_QUERY_FIRST_RECHARGE_GIVE			0x2782	//10114, ��ѯ�ѳ�ֵ�����׳����͵ĳ�ֵid
#define MLS_KILL_MONSTER_EXP					0x2783	//10115, ɱ�ֵþ���
#define MLS_RECEIVE_TASK_REWARD					0x2784	//10116, ��ȡ������(������)
#define MLS_FINISH_TASK_FAIL_NOTIFY				0x2785	//10117, �������ʧ��֪ͨ
#define MLS_PICKUP_BROKEN_DROP					0x2786	//10118, ʰȡ���������
#define MLS_QUERY_MAGICS						0x2787	//10119, ��ѯ��ɫӵ��ħ����
#define MLS_GAIN_MAGIC							0x2788	//10120, ���ħ����
#define MLS_GAIN_MAGIC_NOTIFY					0x2789	//10121, ���ħ����֪ͨ
#define MLS_FRAGMENT_UPDATE_NOTIFY				0x278A	//10122, װ����Ƭˢ��֪ͨ
#define MLS_EQUIP_POS_STRENGTHEN				0x278B	//10123, װ��λǿ��
#define MLS_EQUIP_POS_RISE_STAR					0x278C	//10124, װ��λ����
#define MLS_EQUIP_POS_INLAID_GEM				0x278D	//10125, װ��λ��Ƕ��ʯ
#define MLS_EQUIP_POS_REMOVE_GEM				0x278E	//10126, װ��λȡ����ʯ
#define MLS_GET_EQUIP_POS_ATTR					0x278F	//10127, ��ȡװ��λ����
#define MLS_QUERY_ONLINE_SIGN_IN				0x2790	//10128, ��ѯ��ǰǩ��
#define MLS_ONLINE_SIGN_IN						0x2791	//10129, ����ǩ��
#define MLS_QUERY_RECEIVED_ACTIVITY				0x2792	//10130, ��ѯ����ȡ�
#define MLS_RECEIVED_ACTIVITY					0x2793	//10131, ��ȡ�
#define MLS_QUERY_CUR_OPEN_CHAPTER				0x2794	//10132, ��ѯ��ǰ�����½�
#define MLS_INST_SWEEP							0x2795	//10133, ����ɨ��
#define MLS_RECEIVE_CASH_CODE_REWARD			0x2796	//10134, ��ȡ�һ��뽱��
#define MLS_QUERY_UNLOCK_CONTENT				0x2797	//10135, ��ѯ�ѽ�������
#define MLS_CONTENT_UNLOCK						0x2798	//10136, ���ݽ���
#define MLS_GET_ROULETTE_REMAIN_REWARD			0x2799	//10137, ��ȡ����ʣ�ཱƷ
#define MLS_EXTRACT_ROULETTE_REWARD				0x279A	//10138, ��ȡ���̽�Ʒ
#define MLS_FETCH_RECHARGE_ORDER				0x279B	//10139, ��ȡ��ֵ����
#define MLS_GET_RECHARGE_TOKEN					0x279C	//10140, ��ȡ��ֵ����
#define MLS_UPDATE_ROULETTE_BIG_REWARD			0x279D	//10141, ˢ�����̴�
#define MLS_FETCH_ROULETTE_BIG_REWARD			0x279E	//10142, ��ȡ���̴�
#define MLS_GET_ACTIVE_INST_CHALLENGE_TIMES		0x279F	//10143, ��ȡ�����������ս����
#define MLS_QUERY_VIT_EXCHANGE					0x27A1	//10145, ��ѯ���������һ�����
#define MLS_VIT_EXCHANGE						0x27A2	//10146, �����һ�
#define MLS_QUERY_ROLE_DAILY_SIGN				0x27A3	//10147, ��ѯ��ɫÿ��ǩ��
#define MLS_ROLE_DAILY_SIGN						0x27A4	//10148, ��ɫÿ��ǩ��
#define MLS_CHAIN_SOUL_FRAGMENT_UPDATE_NOTIFY	0x27A5	//10149, ������Ƭˢ��֪ͨ
#define MLS_QUERY_CHAIN_SOUL_FRAGMENT			0x27A6	//10150, ��ѯ������Ƭ
#define MLS_QUERY_CHAIN_SOUL_POS				0x27A7	//10151, ��ѯ���겿��
#define MLS_CHAIN_SOUL_POS_UPGRADE				0x27A8	//10152, �������겿��
#define MLS_ATTACK_WORLD_BOOS					0x27A9	//10153, ��������BOSS
#define MLS_QUERY_WORLD_BOSS_RANK				0x27AA	//10154, ��ѯ����BOSS����
#define MLS_WASKILLED_BY_WORLDBOSS				0x27AB	//10155, ������Bossɱ��
#define MLS_RESURRECTION_IN_WORLDBOSS			0x27AC	//10156, ������Boss�����и���
#define MLS_QUERY_WORLDBOSS_INFO				0x27AD	//10157, ��ѯ����Boss��Ϣ
#define MLS_MATCHING_LADDER						0x27AE	//10158, ƥ���������
#define MLS_CHALLENGE_LADDER					0x27AF	//10159, ��ս�������
#define MLS_GET_LADDER_ROLE_INFO				0x27B0	//10160, ��ѯ���������Ϣ
#define MLS_ELIMINATE_LADDER_CD					0x27B1	//10161, ��������CD
#define MLS_REFRESH_LADDER_RANK					0x27B2	//10162, ˢ����������
//#define MLS_FINISH_LADDER_CHALLENGE			0x27B3	//10163, ���������ս
#define MLS_QUERY_ALL_ROLES						0x27B4	//10164, ��ѯ���н�ɫ
#define MLS_QUERY_HAS_ACTIVITY_INST_TYPE		0x27B5	//10165, ��ѯӵ�еĻ��������
#define MLS_QUERY_WEEKFREE_ACTOR				0x27B6	//10166, ��ѯ�����ɫ
#define MLS_QUERY_COMPMODE_ENTER_NUM			0x27B7	//10167, ��ѯ����ģʽ�������
#define MLS_BUY_COMPMODE_ENTER_NUM				0x27B8	//10168, ���򾺼�ģʽ�������
#define MLS_QUERY_1V1_ENTER_NUM					0x27B9	//10169, ��ѯ1v1ģʽ�������
#define MLS_BUY_1V1_ENTER_NUM					0x27BA	//10170, ����1v1�������
#define MLS_MESSAGETIPS_NOTIFY					0x27BB	//10171, ��Ϣ��ʾ֪ͨ

//add by hxw 20151006
#define MSL_QUERY_LV_RANK					0x2901	//10497, ��ѯ�ȼ����н�����ȡ��Ϣ
#define MSL_QUERY_INNUM_RANK_INFO				0x2902  //10498, ��ѯ������ս�������а�
#define MSL_QUERY_INNUM_RANK					0x2903  //10499, ��ѯ�Ƿ���Ҫ��ȡ������ս����
#define MSL_GET_LV_RANK_REWARD					0x2904	//10500, ��ȡLV���н���
#define MSL_GET_INNUM_REWARD					0x2905	//10501, ��ȡ������ս�������н���
#define MSL_QUERY_STRONGER_INFO					0x2906	//10502, ��ѯǿ�߽�������ȡ��Ϣ
#define MSL_GET_STRONGER_REWARD					0x2907	//10503, ��ȡǿ�߽���
#define MSL_QUERY_ONLINE_INFO					0x2908	//10504, ��ѯ���߽�����ȡ״��
#define MSL_GET_ONLINE_REWARD					0x2909  //10505, ��ȡ���߽���
#define MSL_QUERY_LV_RANK_INFO					0x290A  //10506, ��ѯ�ȼ����а���Ϣ

//end
/************************************************************************
0x9C40------0x9CA4���ػ����̷��������룩	///
*************************************************************************/
#define DAEMON_LOGIN							0x9C40	// 40000����½�ػ����̷�����

/************************************************************************
���������������
************************************************************************/
#define	GM_USER_LOGIN					0xC350 //50000 ��¼���������
#define	GM_USER_LOGINOUT				0xC351 //50001 �ǳ����������
#define	GM_ACCOUNT_CREATE				0xC352 //50002 �ʺŴ�����
#define	GM_LOGIN_ACCOUNT				0xC353 //50003 ��¼�û���(��ɫ��֮��)
#define	GM_ADD_NEW_ACCOUNT				0xC354 //50004 �����û���
#define	GM_ACTIVE_ACCOUNT				0xC355 //50005 ��Ծ�û���
#define	GM_ADDUP_NEW					0xC356 //50006 �ۼ�
#define	GM_ADD_NEW_DEVICE				0xC357 //50007 �����豸
#define	GM_AVERAGE_ONLINE_TIME			0xC358 //50008 ƽ������ʱ��
#define	GM_AVERAGE_PLAY_GAME			0xC359 //50009 ƽ����Ϸ����
#define	GM_MAX_ONLINE_USER_COUNT		0xC35B //50011 �����������
#define	GM_AVERAGE_ONLINE_USER_COUNT	0xC35C //50012 ƽ����������
#define	GM_PAYMENT_USER_COUNT			0xC35D //50013 �����û���
#define	GM_PAYMENT_COUNT				0xC35E //50014 ���Ѵ���
#define	GM_ADD_NEW_PAYMENT_COUNT		0xC36F //50015 ���������û���
#define	GM_ADDUP_PAYMENT_COUNT			0xC360 //50016 �ۼƸ����û���
#define	GM_ADD_NEW_PAYMENT_MONEY		0xC361 //50017 �������ѽ��
#define	GM_PAYMENT_MONEY				0xC362 //50018 ���ѽ��
#define	GM_ADDUP_PAYMENT_MONEY			0xC363 //50019 �ۼƸ��ѽ��
#define	GM_DEVICE_ACTIVE				0xC365 //50021 �豸����
#define	GM_COST_TOTAL					0xC366 //50022 �����ܽ��
#define	GM_SINGLE_ACTOR_COST_TOTAL		0xC367 //50023 ������ɫ�����ܽ��
#define	GM_ACCOUNT_DETAIL				0xC368 //50024 �ʺ�����
#define	GM_ACTOR_DETAIL					0xC369 //50025 ��ɫ����
#define	GM_SINGLE_ACCOUNT_DETAIL		0xC36A //50026 �����ʺ�����
#define	GM_SINGLE_ACTOR_DETAIL			0xC36B //50027 ������ɫ����
#define	GM_PLAT_PART_INFO				0xC36C //50028 ��ȡ����������Ϣ

#define GM_NEW_STATISTICS				0xC36D //50029 ʵʱ����ͳ��
#define GM_ORDER_TRACKING				0xC36E //50030 ������ѯ
#define GM_ORDER_STATISTICS				0xC36F //50031 ��ֵ����ͳ��
#define GM_RETENTION_STATISTICS			0xC370 //50032 ����ͳ��(���ա����ա����ա���30�ա�������)
#define GM_ONLINE_REALTIME				0xC371 //50033 ����ͳ��_ʵʱ����
#define GM_ONLINE_STATISTICS            0xC372 //50034 ����ͳ��_��������ͳ��;
#define GM_LEVEL_DISTRIBUTION           0xC373 //50035 �ȼ��ֲ�

#define LINK_MAU_SERVER(cmdCode)	((0xEA60) <= (cmdCode))
/************************************************************************
0xEA60------��mau��Ϸ���������룩
*************************************************************************/
#define MAU_CREATE_TABLE				0xEA60	//60000,��Ҵ�������
#define MAU_JION_TABLE					0xEA61	//60001,��ҽ�������
#define MAU_LEAVE_TABLE					0xEA62	//60002,����뿪����
#define MAU_USER_EXIT					0xEA63	//60003,�û��˳�
#define MAU_PLAYER_READY			    0xEA64	//60004,���׼��
//#define CHAT_USER_EXIT				    0xEA64	//60004,�û��˳�
//#define CHAT_USER_OFFLINE_NOTIFY	    0xEA65	//60005,�û�������֪ͨ

#endif