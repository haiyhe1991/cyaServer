#ifndef __SERVICE_CMD_CODE_H__
#define __SERVICE_CMD_CODE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#define LINK_OTHER_LOGIC_SERVER(cmdCode)    ((0x6AA4) <= (cmdCode) && (0x6B6B) >= (cmdCode))

/************************************************************************
0x61A9------（管理服务器命令码）
*************************************************************************/
//管理用户部分
#define MANS_USER_LOGIN					0x61A9	//25001, 登录登录
#define MANS_USER_LOGOUT				0x61AA	//25002, 用户退出

//验证服务器部分
#define LCS_QUERY_ACCOUNT_PARTITION		0x61B3	//25011, 查询玩家账户分区
#define LCS_QUERY_PLAYER_ACCOUNT		0x61B4	//25012, 查询玩家账户
#define LCS_QUERY_GAME_PARTITION		0x61B5	//25013, 查询游戏分区
#define LCS_ADD_PARTITION				0x61B6	//25014, 增加分区
#define LCS_DEL_PARTITION				0x61B7	//25015, 删除分区
#define LCS_MODIFY_PARTITION			0x61B8	//25016, 修改分区
#define LCS_SET_RECOMMEND_PARTITION		0x61B9	//25017, 设置推荐分区
#define LCS_RELOAD_PARTITION			0x61BA	//25018, 重新载入分区
#define LCS_START_LINKER_SERVE			0x61BB	//25019, 开启link服务
#define LCS_STOP_LINKER_SERVE			0x61BC	//25020, 停止link服务
#define LCS_START_PARTITION_SERVE		0x61BD	//25021, 开启分区
#define LCS_STOP_PARTITION_SERVE		0x61BE	//25022, 停止分区


//游戏逻辑服务器部分
#define LGS_QUERY_PLAYER_LIST			0x61E5	//25061, 查询玩家列表
#define LGS_SEND_PLAYER_EMAIL			0x61E6	//25062, 向用户发送邮件

/************************************************************************
0x6AA4------0x6B6B（排行服务命令码）
*************************************************************************/
#define RANK_QUERY_ROLE_INFO		    0x6AA4	//27300, 查询玩家的排行信息
#define RANK_QUERY_ROLE_RANK		    0x6AA5	//27301, 查询当前玩家的某项排行信息
#define RANK_SEND_LOG_INFO		        0x6AA6	//27302, 发送log信息到排行服务器

/************************************************************************
0x6B6C------0x7530（聊天服务命令码）
*************************************************************************/
#define CHAT_JOIN_CHANNEL			    0x6B6C	//27500,加入频道
#define CHAT_LEAVE_CHANNEL			    0x6B6D	//27501,离开频道
#define CHAT_SEND_MESSAGE			    0x6B6E	//27502,发送信息
#define CHAT_RECV_MESSAGE			    0x6B6F	//27503,收到信息
#define CHAT_USER_EXIT				    0x6B70	//27504,用户退出
#define CHAT_USER_OFFLINE_NOTIFY	    0x6B71	//27505,用户不在线通知


/************************************************************************
0x7531------（验证服务命令码）
*************************************************************************/
#define LICENCE_LINKER_LOGIN					0x7531	//30001,link服务器登录
// #define LICENCE_START_PARTITION_SERVE		0x7532	//30002,开启分区
// #define LICENCE_STOP_PARTITION_SERVE			0x7533	//30003,停止分区
#define LICENCE_LINKER_GET_TOKEN				0x7534	//30004,从link获取token
#define LICENCE_LINKER_QUERY_PARTITION_STATUS	0x7535	//30005,link服务器查询当前所在分区状态
#define LICENCE_LINKER_REPORT_CONNECTIONS		0x7536	//30006,link服务器上报当前连接数
#define LICENCE_LINKER_ROLE_CHANGE				0x7537	//30007,link服务器汇报角色创建或删除

#define LICENCE_USER_LOGIN						0x7538	//30008,用户登录
#define LICENCE_USER_QUERY_PARTITION			0x7539	//30009,用户查询分区信息
#define LICENCE_USER_ENTER_PARTITION			0x753A	//30010,用户进入分区请求
#define LICENCE_USER_REGIST						0x753B	//30011,用户注册

// #define LICENCE_PARTITION_ADD				0x753C	//30012,增加分区
// #define LICENCE_PARTITION_DEL				0x753D	//30013,删除分区
// #define LICENCE_PARTITION_MODIFY				0x753E	//30014,修改分区
// #define LICENCE_PARTITION_QUERY_INFO			0x753F	//30015,查询分区
// #define LICENCE_PARTITION_SET_RECOMMEND		0x7540	//30016,设置推荐分区
// #define LICENCE_PARTITION_RELOAD				0x7541	//30017,重新载入分区

// #define LICENCE_START_LINKER_SERVE			0x7542	//30018,开启link服务
// #define LICENCE_STOP_LINKER_SERVE			0x7543	//30019,停止link服务

#define LICENCE_LINKER_USER_CHECK				0x7544	//30020,linker通知验证服务器用户验证通过
#define LICENCE_NOTIFY_USER_OFFLINE				0x7545	//30021,通知linker要下线的用户
#define LICENCE_QUERY_USER_LOGIN				0x7546	//30022,查询用户登陆信息
#define LICENCE_GET_REPORT_TIMER				0x7547	//30023,获取上报linker连接数时间间隔(单位:秒)

#define LICENCE_QUERY_UNLOCK_JOBS				0x7548	//30024,查询解锁职业
#define LICENCE_LINKER_QUERY_UNLOCK_JOBS		0x7549	//30025,Linker查询解锁职业


/************************************************************************
0x4E20------0x55F0（Linker服务命令码）	///
*************************************************************************/
#define LINKER_LOGIN							0x4E20	// 20000，登陆Linker服务器
#define LINKER_LOGOUT							0x4E21	// 20001，登出Linker服务器
#define LINKER_HEARTBEAT						0x4E22	// 20002, 心跳
#define LINKER_GET_ONLINE_CONNECT_NUM			0x4E23	// 20003, 获取当前Linker在线人数

/************************************************************************
0x2710------0x4E1F（主逻辑服务命令码）	///
*************************************************************************/
#define MLS_QUERY_ROLES							0x2710	//10000, 查询角色信息
#define MLS_ENTER_ROLE							0x2711	//10001, 进入角色
#define MLS_QUERY_BACKPACK						0x2712	//10002, 查询背包
#define MLS_QUERY_ACHIEVEMENT					0x2713	//10003, 查询成就
#define MLS_QUERY_EMAIL_CONTENT					0x2714	//10004, 查询邮件内容
#define MLS_CREATE_ROLE							0x2715	//10005, 创建角色
#define MLS_QUERY_FRIEND						0x2716	//10006, 查询好友
#define MLS_ADD_FRIEND							0x2717	//10007, 添加好友
#define MLS_DEL_FRIEND							0x2718	//10008, 删除好友
#define MLS_QUERY_FINISHED_HELP					0x2719	//10009, 查询已完成新手引导信息
#define MLS_QUERY_FINISHED_INST					0x271A	//10010, 查询已完成副本信息
#define MLS_BUFF_OVER_NOTIFY					0x271B	//10011, buf结束通知
#define MLS_SAVE_GEAR_DESTROY_STATE				0x271C	//10012, 保存机关破坏状态
#define MLS_SAVE_ROLE_ATTR						0x271D	//10013, 保存属性点
#define MLS_FINISH_ROLE_HELP					0x271E	//10014, 完成新手引导
#define MLS_BACKPACK_MOD_NOTIFY					0x271F	//10015, 背包更改主动通知
#define MLS_QUERY_CUR_CASH						0x2720	//10016, 查询当前兑换表
#define MLS_QUERY_ROLE_APPEARANCE				0x2721	//10017, 查询角色外型信息
#define MLS_QUERY_ROLE_EXPLICIT					0x2722	//10018, 查询角色详细信息
#define MLS_QUERY_GIFT_BAG						0x2723	//10019, 查询礼品包
#define MLS_QUERY_ACCEPTED_TASK					0x2724	//10020, 查询已接受任务
#define MLS_DELTA_TOKEN							0x2725	//10021, 充值代币
#define MLS_DELTA_NOTIFY						0x2726	//10022, 充值通知
#define MLS_SELL_PROPERTY						0x2727	//10023, 出售道具
#define MLS_FLUSH_CASH							0x2728	//10024, 刷新兑换表
#define MLS_DROP_PROPERTY						0x2729	//10025, 丢弃道具
#define MLS_CASH_PROPERTY						0x272A	//10026, 兑换道具
#define MLS_DROP_TASK							0x272B	//10027, 放弃任务
#define MLS_NEW_EMAIL_NOTIFY					0x272C	//10028, 服务器主动通知有新邮件
#define MLS_PICKUP_INST_BOX						0x272D	//10029, 拾取副本宝箱
#define MLS_BUY_STORAGE_SPACE					0x272E	//10030, 购买背包或仓库储存空间
#define MLS_ACCEPT_TASK							0x272F	//10031, 接受任务
#define MLS_ENTER_INST							0x2730	//10032, 进入副本
#define MLS_RECEIVE_REWARD						0x2731	//10033, 领取成就奖励
#define MLS_UPDATE_NOTIFY						0x2732	//10034, 升级通知
#define MLS_USE_PROPERTY						0x2733	//10035, 使用道具
#define MLS_RECEIVE_EMAIL_ATTACHMENT			0x2734	//10036, 收取邮件附件
#define MLS_FINISH_INST							0x2735	//10037, 完成某副本
#define MLS_FETCH_ROBOT							0x2736	//10038, 获取机器人数据
//#define MLS_INLAID_GEM							0x2737	//10039, 镶嵌宝石
//#define MLS_MOD_ACCEPTED_TASK_STATE				0x2738	//10040, 修改已接受任务状态
#define MLS_QUERY_EMAIL_LIST					0x2739	//10041, 邮件查询列表
#define MLS_MOD_ROLE_ATTR						0x273A	//10042, 重置属性点
#define MLS_LOAD_EQUIPMENT						0x273B	//10043, 载入装备
#define MLS_UNLOAD_EQUIPMENT					0x273C	//10044, 卸载装备
#define MLS_LEAVE_ROLE							0x273D	//10045, 退出角色
#define MLS_DEL_ROLE							0x273E	//10046, 删除角色
//#define MLS_REMOVE_GEM							0x273F	//10047, 取出宝石
#define MLS_QUERY_GUILD_NAME					0x2740	//10048, 查询公会名称
#define MLS_USER_EXIT							0x2741	//10049, 账号退出
//#define MLS_QUERY_INST_RANK						0x2742	//10050, 查询某副本排行信息
#define MLS_PERSONAL_TIMER_TASK					0x2743	//10051, 个人定时任务请求
#define MLS_PERSONAL_TIMER_TASK_FIN				0x2744	//10052, 个人定时任务完成
#define MLS_FETCH_INST_AREA_DROPS				0x2745	//10053, 获取副本区域怪掉落
#define MLS_ROLE_UPGRADE_NOTIFY					0x2746	//10054, 角色升级通知
#define MLS_QUERY_ROLE_SKILLS					0x2747	//10055, 查询角色已有技能
#define MLS_SKILL_UPGRADE						0x2748	//10056, 技能升级
#define MLS_CHANGE_FACTION						0x2749	//10057, 改变阵营
#define MLS_LOAD_SKILL							0x274A	//10058, 装载技能
#define MLS_CANCEL_SKILL						0x274B	//10059, 取消技能
//#define MLS_SET_SKILL_CLIENT_ID					0x274C	//10060, 设置技能客户端id
#define MLS_UPDATE_TASK_TARGET					0x274D	//10061, 更新已接任务目标
#define MLS_COMMIT_TASK_FINISH					0x274E	//10062, 提交任务完成
#define MLS_TASK_STATUS_UPDATE_NOTIFY			0x274F	//10063, 任务状态更新通知
#define MLS_QUERY_FIN_TASK_HISTORY				0x2750	//10064, 查询已完成任务历史记录
#define MLS_GET_DAILY_TASK						0x2751	//10065, 获取日常任务
//#define MLS_STRENGTHEN_EQUIPMENT				0x2752	//10066, 强化装备
//#define MLS_EQUIPMENT_RISE_STAR					0x2753	//10067, 装备升星
#define MLS_EQUIPMENT_COMPOSE					0x2754	//10068, 装备合成
//#define MLS_EQUIPMENT_CLEAR_STAR				0x2755	//10069, 装备消星
//#define MLS_EQUIPMENT_INHERIT					0x2756	//10070, 装备继承
//#define MLS_EQUIPMENT_STRENGTHEN_INHERIT		0x2757	//10071, 装备强化继承
//#define MLS_EQUIPMENT_STRENGTHEN_CLEAR			0x2758	//10072, 装备强化消除
#define MLS_GEM_COMPOSE							0x2759	//10073, 宝石合成
#define MLS_GEM_DECOMPOSE						0x275A	//10074, 宝石分解
#define MLS_INST_OCCUPY_PUSH_NOTIFY				0x275B	//10075, 副本占领推送通知
#define MLS_PROP_BUY_BACK						0x275C	//10076, 物品回购
#define MLS_QUERY_ROLE_SELL_PROP_RCD			0x275D	//10077, 查询角色出售物品记录
//#define MLS_FRAGMENT_COMPOSE_EQUIP				0x275E	//10078, 碎片合成装备
#define MLS_VIT_UPDATE_NOTIFY					0x275F	//10079, 体力更新通知
#define MLS_BACKPACK_CLEAN_UP					0x2760	//10080, 背包整理
#define MLS_GET_ROLE_OCCUPY_INST				0x2761	//10081, 获取角色占领副本
#define MLS_SET_ROLE_TITLE_ID					0x2762	//10082, 设置角色职业称号id
#define MLS_DEL_EMAIL							0x2763	//10083, 删除角色邮件
#define MLS_MALL_BUY_GOODS						0x2764	//10084, 商城购买物品
#define MLS_QUERY_ROLE_DRESS					0x2765	//10085, 查询角色拥有时装
#define MLS_WEAR_DRESS							0x2766	//10086, 穿戴时装
#define MLS_PICKUP_INST_MONSTER_DROP			0x2767	//10087, 拾取副本怪掉落物品
#define MLS_GOLD_UPDATE_NOTIFY					0x2768	//10088, 角色金币更新通知
#define MLS_TOKEN_UPDATE_NOTIFY					0x2769	//10089, 角色代币更新通知
#define MLS_EXP_UPDATE_NOTIFY					0x276A	//10090, 角色经验更新通知
#define MLS_BACKPACK_UPDATE_NOTIFY				0x276B	//10091, 角色背包更新通知
#define MLS_RAND_INST_OPEN_NOTIFY				0x276C	//10092, 随机副本开启通知
#define MLS_RAND_INST_CLOSE_NOTIFY				0x276D	//10093, 随机副本关闭通知
#define MLS_GET_PERSONAL_TRANSFER_ENTER			0x276E	//10094, 获取单人随机副本传送阵进入人数
#define MLS_QUERY_EQUIP_FRAGMENT				0x276F	//10095, 查询装备碎片
//#define MLS_EQUIP_FRAGMENT_COMPOSE				0x2770	//10096, 装备碎片合成
#define MLS_ROLE_REVIVE							0x2771	//10097, 角色复活
#define MLS_GET_OPEN_PERSONAL_RAND_INST			0x2772	//10098, 获取已打开单人随机副本
#define MLS_SYS_NOTICE_NOTIFY					0x2773	//10099, 系统公告通知
#define MLS_QUERY_GOLD_EXCHANGE					0x2774	//10100, 查询当天金币兑换次数
#define MLS_GOLD_EXCHANGE						0x2775	//10101, 金币兑换
#define MLS_FRIEND_APPLY_NOTIFY					0x2776	//10102, 好友申请通知
#define MLS_FRIEND_APPLY_CONFIRM				0x2777	//10103, 好友申请确认
#define MLS_AGREE_FRIEND_APPLY_NOTIFY			0x2778	//10104, 好友申请同意通知
#define MLS_QUERY_FRIEND_APPLY					0x2779	//10105, 查询好友申请
#define MLS_GET_RECOMMEND_FRIEND_LIST			0x277A	//10106, 获取推荐好友列表
#define MLS_SEARCH_PLAYER						0x277B	//10107, 搜索玩家
#define MLS_FRIEND_ONLINE_NOTIFY				0x277C	//10108, 好友上线通知
#define MLS_FRIEND_OFFLINE_NOTIFY				0x277D	//10109, 好友下线通知
#define MLS_REFUSE_FRIEND_APPLY_NOTIFY			0x277E	//10110, 拒绝好友申请通知
#define MLS_FRIEND_DEL_NOTIFY					0x277F	//10111, 好友删除通知
#define MLS_NOT_RECEIVE_VIP_REWARD				0x2780	//10112, 未领取奖励的VIP等级
#define MLS_RECEIVE_VIP_REWARD					0x2781	//10113, 领取VIP奖励
#define MLS_QUERY_FIRST_RECHARGE_GIVE			0x2782	//10114, 查询已充值且有首充赠送的充值id
#define MLS_KILL_MONSTER_EXP					0x2783	//10115, 杀怪得经验
#define MLS_RECEIVE_TASK_REWARD					0x2784	//10116, 领取任务奖励(交任务)
#define MLS_FINISH_TASK_FAIL_NOTIFY				0x2785	//10117, 完成任务失败通知
#define MLS_PICKUP_BROKEN_DROP					0x2786	//10118, 拾取破碎物掉落
#define MLS_QUERY_MAGICS						0x2787	//10119, 查询角色拥有魔导器
#define MLS_GAIN_MAGIC							0x2788	//10120, 获得魔导器
#define MLS_GAIN_MAGIC_NOTIFY					0x2789	//10121, 获得魔导器通知
#define MLS_FRAGMENT_UPDATE_NOTIFY				0x278A	//10122, 装备碎片刷新通知
#define MLS_EQUIP_POS_STRENGTHEN				0x278B	//10123, 装备位强化
#define MLS_EQUIP_POS_RISE_STAR					0x278C	//10124, 装备位升星
#define MLS_EQUIP_POS_INLAID_GEM				0x278D	//10125, 装备位镶嵌宝石
#define MLS_EQUIP_POS_REMOVE_GEM				0x278E	//10126, 装备位取出宝石
#define MLS_GET_EQUIP_POS_ATTR					0x278F	//10127, 获取装备位属性
#define MLS_QUERY_ONLINE_SIGN_IN				0x2790	//10128, 查询当前签到
#define MLS_ONLINE_SIGN_IN						0x2791	//10129, 上线签到
#define MLS_QUERY_RECEIVED_ACTIVITY				0x2792	//10130, 查询已领取活动
#define MLS_RECEIVED_ACTIVITY					0x2793	//10131, 领取活动
#define MLS_QUERY_CUR_OPEN_CHAPTER				0x2794	//10132, 查询当前开放章节
#define MLS_INST_SWEEP							0x2795	//10133, 副本扫荡
#define MLS_RECEIVE_CASH_CODE_REWARD			0x2796	//10134, 领取兑换码奖励
#define MLS_QUERY_UNLOCK_CONTENT				0x2797	//10135, 查询已解锁内容
#define MLS_CONTENT_UNLOCK						0x2798	//10136, 内容解锁
#define MLS_GET_ROULETTE_REMAIN_REWARD			0x2799	//10137, 获取轮盘剩余奖品
#define MLS_EXTRACT_ROULETTE_REWARD				0x279A	//10138, 抽取轮盘奖品
#define MLS_FETCH_RECHARGE_ORDER				0x279B	//10139, 获取充值订单
#define MLS_GET_RECHARGE_TOKEN					0x279C	//10140, 获取充值代币
#define MLS_UPDATE_ROULETTE_BIG_REWARD			0x279D	//10141, 刷新轮盘大奖
#define MLS_FETCH_ROULETTE_BIG_REWARD			0x279E	//10142, 获取轮盘大奖
#define MLS_GET_ACTIVE_INST_CHALLENGE_TIMES		0x279F	//10143, 获取活动副本当天挑战次数
#define MLS_QUERY_VIT_EXCHANGE					0x27A1	//10145, 查询当天体力兑换次数
#define MLS_VIT_EXCHANGE						0x27A2	//10146, 体力兑换
#define MLS_QUERY_ROLE_DAILY_SIGN				0x27A3	//10147, 查询角色每日签到
#define MLS_ROLE_DAILY_SIGN						0x27A4	//10148, 角色每日签到
#define MLS_CHAIN_SOUL_FRAGMENT_UPDATE_NOTIFY	0x27A5	//10149, 炼魂碎片刷新通知
#define MLS_QUERY_CHAIN_SOUL_FRAGMENT			0x27A6	//10150, 查询炼魂碎片
#define MLS_QUERY_CHAIN_SOUL_POS				0x27A7	//10151, 查询炼魂部件
#define MLS_CHAIN_SOUL_POS_UPGRADE				0x27A8	//10152, 升级炼魂部件
#define MLS_ATTACK_WORLD_BOOS					0x27A9	//10153, 攻击世界BOSS
#define MLS_QUERY_WORLD_BOSS_RANK				0x27AA	//10154, 查询世界BOSS排名
#define MLS_WASKILLED_BY_WORLDBOSS				0x27AB	//10155, 被世界Boss杀死
#define MLS_RESURRECTION_IN_WORLDBOSS			0x27AC	//10156, 在世界Boss副本中复活
#define MLS_QUERY_WORLDBOSS_INFO				0x27AD	//10157, 查询世界Boss信息
#define MLS_MATCHING_LADDER						0x27AE	//10158, 匹配天梯玩家
#define MLS_CHALLENGE_LADDER					0x27AF	//10159, 挑战天梯玩家
#define MLS_GET_LADDER_ROLE_INFO				0x27B0	//10160, 查询天梯玩家信息
#define MLS_ELIMINATE_LADDER_CD					0x27B1	//10161, 消除天梯CD
#define MLS_REFRESH_LADDER_RANK					0x27B2	//10162, 刷新天梯排行
//#define MLS_FINISH_LADDER_CHALLENGE			0x27B3	//10163, 完成天梯挑战
#define MLS_QUERY_ALL_ROLES						0x27B4	//10164, 查询所有角色
#define MLS_QUERY_HAS_ACTIVITY_INST_TYPE		0x27B5	//10165, 查询拥有的活动副本类型
#define MLS_QUERY_WEEKFREE_ACTOR				0x27B6	//10166, 查询周免角色
#define MLS_QUERY_COMPMODE_ENTER_NUM			0x27B7	//10167, 查询竞技模式进入次数
#define MLS_BUY_COMPMODE_ENTER_NUM				0x27B8	//10168, 购买竞技模式进入次数
#define MLS_QUERY_1V1_ENTER_NUM					0x27B9	//10169, 查询1v1模式进入次数
#define MLS_BUY_1V1_ENTER_NUM					0x27BA	//10170, 购买1v1进入次数
#define MLS_MESSAGETIPS_NOTIFY					0x27BB	//10171, 消息提示通知

//add by hxw 20151006
#define MSL_QUERY_LV_RANK					0x2901	//10497, 查询等级排行奖励领取信息
#define MSL_QUERY_INNUM_RANK_INFO				0x2902  //10498, 查询副本挑战次数排行榜
#define MSL_QUERY_INNUM_RANK					0x2903  //10499, 查询是否需要领取副本挑战排行
#define MSL_GET_LV_RANK_REWARD					0x2904	//10500, 获取LV排行奖励
#define MSL_GET_INNUM_REWARD					0x2905	//10501, 获取副本挑战次数排行奖励
#define MSL_QUERY_STRONGER_INFO					0x2906	//10502, 查询强者奖励的领取信息
#define MSL_GET_STRONGER_REWARD					0x2907	//10503, 获取强者奖励
#define MSL_QUERY_ONLINE_INFO					0x2908	//10504, 查询在线奖励领取状况
#define MSL_GET_ONLINE_REWARD					0x2909  //10505, 获取在线奖励
#define MSL_QUERY_LV_RANK_INFO					0x290A  //10506, 查询等级排行榜信息

//end
/************************************************************************
0x9C40------0x9CA4（守护进程服务命令码）	///
*************************************************************************/
#define DAEMON_LOGIN							0x9C40	// 40000，登陆守护进程服务器

/************************************************************************
管理服务器命令码
************************************************************************/
#define	GM_USER_LOGIN					0xC350 //50000 登录管理服务器
#define	GM_USER_LOGINOUT				0xC351 //50001 登出管理服务器
#define	GM_ACCOUNT_CREATE				0xC352 //50002 帐号创建数
#define	GM_LOGIN_ACCOUNT				0xC353 //50003 登录用户数(角色数之和)
#define	GM_ADD_NEW_ACCOUNT				0xC354 //50004 新增用户数
#define	GM_ACTIVE_ACCOUNT				0xC355 //50005 活跃用户数
#define	GM_ADDUP_NEW					0xC356 //50006 累计
#define	GM_ADD_NEW_DEVICE				0xC357 //50007 新增设备
#define	GM_AVERAGE_ONLINE_TIME			0xC358 //50008 平均在线时间
#define	GM_AVERAGE_PLAY_GAME			0xC359 //50009 平均游戏次数
#define	GM_MAX_ONLINE_USER_COUNT		0xC35B //50011 最大在线人数
#define	GM_AVERAGE_ONLINE_USER_COUNT	0xC35C //50012 平均在线人数
#define	GM_PAYMENT_USER_COUNT			0xC35D //50013 付费用户数
#define	GM_PAYMENT_COUNT				0xC35E //50014 付费次数
#define	GM_ADD_NEW_PAYMENT_COUNT		0xC36F //50015 新增付费用户数
#define	GM_ADDUP_PAYMENT_COUNT			0xC360 //50016 累计付费用户数
#define	GM_ADD_NEW_PAYMENT_MONEY		0xC361 //50017 新增付费金额
#define	GM_PAYMENT_MONEY				0xC362 //50018 付费金额
#define	GM_ADDUP_PAYMENT_MONEY			0xC363 //50019 累计付费金额
#define	GM_DEVICE_ACTIVE				0xC365 //50021 设备激活
#define	GM_COST_TOTAL					0xC366 //50022 消费总金额
#define	GM_SINGLE_ACTOR_COST_TOTAL		0xC367 //50023 单个角色消费总金额
#define	GM_ACCOUNT_DETAIL				0xC368 //50024 帐号详情
#define	GM_ACTOR_DETAIL					0xC369 //50025 角色详情
#define	GM_SINGLE_ACCOUNT_DETAIL		0xC36A //50026 单个帐号详情
#define	GM_SINGLE_ACTOR_DETAIL			0xC36B //50027 单个角色详情
#define	GM_PLAT_PART_INFO				0xC36C //50028 获取渠道分区信息

#define GM_NEW_STATISTICS				0xC36D //50029 实时新增统计
#define GM_ORDER_TRACKING				0xC36E //50030 订单查询
#define GM_ORDER_STATISTICS				0xC36F //50031 充值数据统计
#define GM_RETENTION_STATISTICS			0xC370 //50032 留存统计(次日、三日、七日、第30日、月留存)
#define GM_ONLINE_REALTIME				0xC371 //50033 在线统计_实时在线
#define GM_ONLINE_STATISTICS            0xC372 //50034 在线统计_在线数据统计;
#define GM_LEVEL_DISTRIBUTION           0xC373 //50035 等级分布

#endif