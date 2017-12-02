#ifndef __SERVICE_ERROR_CODE_H__
#define __SERVICE_ERROR_CODE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/************************************************************************
DBServer(数据服务器)错误码定义
*************************************************************************/
#define DBS_OK                  0       //操作成功
#define DBS_ERROR               1       //未知错误

//客户端错误码
#define DBC_INVALID_PARAM       1001    //参数错误
#define DBC_CONNECT_TIMEOUT     1002    //连接数据服务器超时
#define DBC_SERVER_BROKEN       1003    //数据服务器连接断开
#define DBC_RECV_TIMEOUT        1004    //接收数据超时
#define DBC_INVALID_RESPONSE    1005    //收到不能识别的数据格式
#define DBC_ALLOC_FAILED        1006    //分配内存失败

//服务端错误码
#define DBS_ERR_PARAM           2000    /// 参数错误
#define DBS_ERR_COMMAND         2001    /// 不支持的命令
#define DBS_ERR_SQL             2002    /// sql语句执行失败
#define DBS_ERR_TABLE           2003    /// Table不存在
#define DBS_ERR_KEY             2004    /// Key不存在
#define DBS_ERR_FIELD           2005    /// Field不存在
#define DBS_ERR_BUSY            2006    /// mysql繁忙

/**************************************************************************
GWServer(网关服务器)错误码定义
***************************************************************************/

//客户端错误码
#define GWS_OK                  0       //操作成功
#define GWS_ERROR               1       //未知错误
#define GWC_INVALID_PARAM       3001    //参数错误
#define GWC_CONNECT_TIMEOUT     3002    //连接超时
#define GWC_SERVER_BROKEN       3003    //服务器断线
#define GWC_RECV_TIMEOUT        3004    //接收数据超时
#define GWC_SEND_TIMEOUT        3005    //发送数据超时
#define GWC_INVALID_RESPONSE    3006    //收到不能识别的数据格式
#define GWC_ALLOC_FAILED        3007    //分配内存失败

//服务端错误码
#define GWS_LINKER_OFFLINE      4001    //对端不在线
#define GWS_UNKNOWN_SERVERID    4002    //不能识别的服务器编号
#define GWS_UNKNOWN_PACKET      4003    //不能识别的包类型
#define GWS_SESSIONID_IS_EXIST  4004    //会话id已存在
#define GWS_SERVER_NOT_OPEN     4005    //服务器未开启


/**************************************************************************
LinkServer(Link服务器)错误码定义
***************************************************************************/

/// 服务器错误码
#define LINK_OK                 0       //操作成功
#define LINK_ERROR              1       //未知错误
#define LINK_SERVER_TYPE        5001    //请求的服务器不存在
#define LINK_CMD_CODE           5002    //命令码错误
#define LINK_USER_ID            5003    //用户重复
#define LINK_LOGIN_GWS          5004    //登录网关失败
#define LINK_START_SERVICE      5005    //启动服务失败
#define LINK_CONFIG_INIT        5006    //初始化配置信息失败
#define LINK_ACCOUNT_TOKEN      5007    //账户没有验证
#define LINK_TOKEN_INVALID      5008    //账户Token无效或不存在
#define LINK_PAKCET_SIZE        5009    //用户数据包过大
#define LINK_CONNECT_LS         5010    //连接验证服务器失败
#define LINK_LOGIN_LS           5011    //登录验证服务器失败
#define LINK_SOCK_READ          5012    //SOCKET读错误
#define LINK_SOCK_WRITE         5013    //SOCKET写错误
#define LINK_MEMERY             5014    //申请内存失败
#define LINK_START              5015    //启动失败
#define LINK_STOP               5016    //停止失败
#define LINK_STATUS             5017    //获取服务器状态失败
#define LINK_ROLE_ID            5018    //没有进入角色
#define LINK_USER_INVALID       5019    //无效的用户
#define LINK_ROLE_INVALID       5020    //无效的角色

/**************************************************************************
ChatServer(聊天服务器)错误码定义
***************************************************************************/
#define CHAT_OK                         0       //成功
#define CHAT_ERROR                      1       //未知错误
#define CHAT_GWS_LOGIN_FAILED           6001    //登录网关服务器失败
#define CHAT_DBS_LOGIN_FAILED           6002    //登录数据服务器失败
#define CHAT_ID_EXIST_IN_CHANNEL        6003    //id在频道内已存在
#define CHAT_ID_NOT_EXIST_IN_CHANNEL    6004    //id在频道内不存在
#define CHAT_CHANNEL_MEMBERS_EMPTY      6005    //该频道成员为空
#define CHAT_CHANNEL_NOT_ALLOW_EL       6006    //该频道类型不允许进入或离开
#define CHAT_INVALID_CMD_PACKET         6007    //非法的命令数据包
#define CHAT_UNKNOWN_CMD_CODE           6008    //未知的命令码


/**************************************************************************
RankServer(排行服务器)错误码定义
***************************************************************************/
#define RANK_OK                         0       //成功
#define RANK_ERROR                      1       //未知错误
#define RANK_GWS_LOGIN_FAILED           6101    //登录网关服务器失败
#define RANK_DBS_LOGIN_FAILED           6102    //登录数据服务器失败
#define RANK_DBS_READ_FAILED            6103    //读取数据库失败
#define RANK_INVALID_CMD_PACKET         6104    //非法的命令数据包
#define RANK_UNKNOWN_CMD_CODE           6105    //未知的命令码
#define RANK_UNKNOWN_QUERY_TYPE         6106    //未知的查询类型
#define RANK_QUERY_NO_MORE_DATA         6107    //查询已到末尾

/**************************************************************************
LicenceServer(验证服务器)错误码定义
***************************************************************************/
#define LCS_OK                          0       //成功
#define LCS_ERROR                       1       //未知错误
#define LCS_DBS_LOGIN_FAILED            7001    //登录数据服务器失败
#define LCS_INVALID_TOKEN               7002    //非法的令牌
#define LCS_INVALID_CMD_CODE            7003    //不能识别的命令码
#define LCS_LINKER_LOGIN_AGAIN          7004    //link服务器重复登陆
#define LCS_NOT_EXIST_PARTITION         7005    //不存在的分区
#define LCS_LINKER_NOT_LOGIN            7006    //该link服务器未登陆
#define LCS_MAN_SERVER_LOGIN_AGAIN      7007    //外部管理服务器重复登录
#define LCS_NOT_EXIST_USER              7008    //不存在的用户
#define LCS_USER_LOGIN_AGAIN            7009    //用户重复登录
#define LCS_USER_NOT_LOGIN              7010    //用户未登录
#define LCS_READ_DBS_FAILED             7011    //读取数据库失败
#define LCS_ACCOUNT_PSW_WRONG           7012    //账号密码错误
#define LCS_INVALID_SESSION_PACKET      7013    //非法会话数据包
#define LCS_PARTITION_TABLE_EMPTY       7014    //分区信息表为空
#define LCS_ERROR_LOGIC_CALL            7015    //错误的逻辑调用
#define LCS_EXISTED_PARTITION           7016    //已经存在的分区
#define LCS_INVALID_CMD_PACKET          7017    //非法的命令数据包
#define LCS_OVER_MAX_USER_PAYLOAD       7018    //超过最大用户负载
#define LCS_NOT_EXIST_DBS_ID            7019    //不存在的数据服务器id
#define LCS_EXIST_ACCOUNT_NAME			7020	//已经存在的账号名

/**************************************************************************
MLogicServer(主逻辑服务器)错误码定义
***************************************************************************/
#define MLS_OK								0       //成功
#define MLS_ERROR							1       //未知错误
#define MLS_READ_DBS_FAILED					8001    //读取数据库失败
#define MLS_ROLE_NOT_EXIST					8002    //角色未进入游戏
#define MLS_ROLE_IS_EXISTED					8003    //角色已经存在
#define MLS_RESOURCE_NOT_ENOUGH				8004    //系统资源不足
#define MLS_FRIEND_IS_EXISTED				8005    //好友已存在
#define MLS_FRIEND_NOT_EXIST				8006    //好友不存在
#define MLS_DEL_FRIEND_FAILED				8007    //好友失败
#define MLS_ID_NOT_EXIST					8008    //该id不存在
#define MLS_UNKNOWN_CMD_CODE				8009    //不能识别的命令码
#define MLS_ROLE_NICK_IS_EXISTED			8010    //角色昵称已存在
#define MLS_SCRIPT_EXE_FAILED				8011    //脚本执行失败
#define MLS_SCRIPT_RETVAL_INVALID			8012    //脚本返回值不合法
#define MLS_EMAIL_NOT_EXIST					8013    //该邮件不存在
#define MLS_ACCOUNT_NOT_EXIST				8014    //账号不存在
#define MLS_BACKPACK_IS_FULL				8015    //背包已满
#define MLS_INVALID_PACKET					8016    //非法的数据包
#define MLS_ROLE_IS_ONLINE					8017    //角色处于在线状态不能被删除
#define MLS_GET_ROLE_ID_FAILED				8018    //获取角色id失败
#define MLS_ROLE_TYPE_INVALID				8019    //未知的角色类型
#define MLS_GEM_SLICE_NO_INLAID				8020    //宝石片不能镶嵌
#define MLS_GEM_HOLE_IS_INLAID				8021    //宝石孔已镶嵌宝石
#define MLS_PROPERTY_NOT_EXIST				8022    //物品不存在
#define MLS_GEM_NOT_INLAID					8023    //宝石未镶嵌
#define MLS_GUILD_NOT_EXIST					8024    //公会不存在
#define MLS_OVER_MAX_ROLES_THRESHOLD		8025    //角色个数超过最大阀值
#define MLS_OVER_ROLES_MAX_LEVEL			8026    //超过角色的最大等级
#define MLS_SKILL_ID_NOT_EXIST				8027    //该技能不存在
#define MLS_OVER_MAX_SKILL_LEVEL			8028    //超过该技能最大等级
#define MLS_VIT_NOT_ENOUGH					8029    //体力不足
#define MLS_GOLD_NOT_ENOUGH					8030    //金币不足
#define MLS_NOT_EXIST_INST					8031    //不存在的副本
#define MLS_LOWER_INST_LIMIT_LEVEL			8032    //低于副本限制角色等级
#define MLS_PROP_LV_HIGHER_THAN_ROLE		8033    //物品等级高于角色等级
#define MLS_OVER_MAX_INST_ENTER_TIMES		8034    //超过当前副本最大进入次数
#define MLS_NOT_ENTER_INST					8035    //未进入的副本
#define MLS_HP_NOT_ENOUGH					8036    //HP不足
#define MLS_MP_NOT_ENOUGH					8037    //MP不足
#define MLS_ROLE_NOT_HAS_SKILL				8038    //角色没有该种技能
#define MLS_NOT_EXIST_JOB					8039    //不存在的职业
#define MLS_LOWER_SKILL_LIMIT_LEVEL			8040    //低于技能限制角色等级
#define MLS_NOT_EXIST_TASK					8041    //不存在的任务
#define MLS_LOWER_TASK_LIMIT_LEVEL			8042    //低于任务限制角色等级
#define MLS_OVER_MAX_TASK_NUM				8043    //超过当前最大任务接收数量
#define MLS_EXIST_TASK_QUEUE				8044    //该任务已存在接收任务队列中
#define MLS_FIN_TASK_FAILED					8045    //任务完成失败
#define MLS_FIN_INST_FAILED					8046    //副本完成失败
#define MLS_MAX_STRENGTHEN_LEVEL			8047    //达到最高强化等级
#define MLS_MATERIAL_NOT_ENOUGH				8048    //材料不足
#define MLS_STRENGTHEN_FAILED				8049    //强化失败
#define MLS_MAX_STAR_LEVEL					8050    //装备达到最高星级
#define MLS_STAR_LIMIT_LEVEL				8051    //低于升星角色限制等级
#define MLS_PROP_NOT_ALLOW_SELL				8052    //物品不允许出售
#define MLS_NOT_ALLOW_COMPOSE				8053    //不能合成
#define MLS_COMPOSE_FAILED					8054    //合成失败
#define MLS_ALREADY_WEAR_EQUIP				8055    //该位置已穿戴有装备
#define MLS_WEAR_POS_NOT_MATCH				8056    //穿戴位置不匹配
//by hxw 20150901
//#define MLS_NOT_ALLOW_INHERIT				8057    //不允许继承
#define MLS_WEAR_LV_LIMIT					8057	//低于穿戴装备等级限制 
//end
#define MLS_GEM_HOLE_NOT_OPEN				8058    //宝石孔未打开
#define MLS_GEM_AND_HOLE_NOT_MATCH			8059    //宝石和宝石孔不匹配
#define MLS_GEM_HOLE_INVALID				8060    //宝石孔号不合法
#define MLS_GEM_MATERIAL_NOT_MATCH			8061    //宝石合成材料不匹配
#define MLS_NO_COMPOS_QUALITY				8062    //装备合成品质不存在
#define MLS_BOX_NOT_EXIST					8063    //宝箱不存在
#define MLS_NICK_NOT_ALLOW_EMPTY			8064    //昵称不允许为空
#define MLS_NO_PROP_SELL_RCD				8065    //无物品出售记录
#define MLS_PROP_ATTR_IS_FULL				8066    //物品属性已满
#define MLS_GOODS_NOT_EXIST					8067    //商品不存在
#define MLS_CASH_NOT_ENOUGH					8068    //代币不足
#define MLS_OVER_MAX_DRESS_LIMIT			8069    //超过拥有最大时装数量限制
#define MLS_DRESS_NOT_EXIST					8070    //时装不存在
#define MLS_DRESS_OUT_DATE					8071    //时装过期
#define MLS_JOB_DRESS_NOT_MATCH				8072    //职业时装不匹配
#define MLS_VIP_LV_NOT_ENOUGH				8073    //VIP等级不够
#define MLS_INVALID_CHASH_ID				8074    //非法的充值id
#define MLS_EQUIP_STRENGTH_LV0				8075    //装备强化等级为0不能消除
#define MLS_EQUIP_STAR_LV0					8076    //装备星级为0不能消除
#define MLS_TITLE_NOT_EXIST					8077    //称号不存在
#define MLS_NOT_OPEN_RAND_INST				8078    //未开启的随机副本
#define MLS_RAND_INST_FULL					8079    //随机副本人数已满
#define MLS_NOT_EXIST_SUIT					8080    //不存在的套装
#define MLS_INVALID_REVIVE					8081    //非法的复活
#define MLS_OVER_MAX_REVIVE_TIMES			8082    //超过最大复活次数
#define MLS_NOT_EXIST_EXCHANGE_ID			8083    //不存在的金币兑换id
#define MLS_OVER_DAY_EXCHANGE_TIMES			8084    //超过当天最大兑换次数
#define MLS_OVER_MAX_FRIENDS				8085    //超过最大好友数量限制
#define MLS_NOT_FIND_PLAYER					8086    //未查找到玩家信息
#define MLS_ALREADY_RECEIVED_VIP			8087    //已经领取的VIP奖励
#define MLS_NOT_EXIST_MONSTER				8088    //不存在的怪
#define MLS_FINISH_TASK_FAILED				8089    //完成任务失败
#define MLS_ALREADY_FINISHED_HELP			8090    //已完成的新手引导
#define MLS_HAS_INST_DROP_EMAIL				8091    //有副本掉落邮件未领取
#define MLS_BATTLE_AREA_NOT_EXIST			8092    //战斗区域不存在
#define MLS_NOT_EXIST_COMPOSE_ID			8093    //不存在的合成ID
#define MLS_ROLE_LV_OUT_RANGE				8094    //角色等级超出范围
#define MLS_DATETIME_OUT_RANGE				8095    //日期超出范围
#define MLS_NOT_HAS_MAGIC					8096    //无魔导器
#define MLS_UNKNOWN_EQUIP_POS				8097    //未知的装备位
#define MLS_NOT_EXIST_CHAPTER				8098    //不存在的章节
#define MLS_ALREADY_SIGN_IN					8099    //当天已经签到
#define MLS_INVALID_SIGN_IN					8100    //非法的签到
#define MLS_NOT_EXIST_ACTIVITY				8101    //不存在的活动
#define MLS_ACTIVITY_OUT_DATE				8102    //活动过期
#define MLS_ALREADY_RECEIVED_ACTIVITY		8103    //已经领取的活动
#define MLS_INVALID_CASH_CODE				8104	//非法的兑换码
#define MLS_CASH_CODE_EXPIRED				8105	//兑换码过期
#define MLS_INVALID_UNLOCK_ID				8106	//非法的解锁id
#define MLS_INVALID_GIFTBAG_ID				8107	//非法的礼包id
#define MLS_GENERATE_ORDER_FAILED			8108	//生成订单失败
#define MLS_GET_ORDER_RESULT_FAILED			8109	//获取订单结果失败
#define MLS_INVALID_ORDER					8110	//无效的订单
#define MLS_ORDER_NOT_CONFIRMED				8111	//订单处于未确认状态
#define MLS_ALREADY_GET_ORDER				8112	//已经领取过的订单
#define MLS_INST_FAILED_TIMEOVER			8113	//超出副本完成时间
#define MLS_INST_FAILED_DEAD				8114	//角色死亡
#define MLS_INST_FAILED_EXIT				8115	//角色中途退出
#define MLS_INST_FAILED_STONE_BROKEN		8116	//石碑破碎
#define MLS_RECEIVED_BIG_REWARD				8117	//大奖已经领取
#define MLS_NOT_FIN_EXTRACT					8118	//未完成抽奖
#define MLS_ALREADY_FIN_EXTRACT				8119	//已经完成抽奖
#define MLS_INVALID_ACHIEVEMENT_TYPE		8120	//无效成就类型
#define MLS_ACHIEVEMENT_NOT_REACHED			8121	//成就未达成
#define MLS_ACHIEVEMENT_HAVE_RECEIVED		8122	//成就已领取
#define MLS_TODAY_HAS_SIGNED				8123	//今日已签到
#define MLS_INVALID_CHAINSOUL_POS			8124	//无效的炼魂部件
#define MLS_CHAINSOUL_FRAGMENT_NOT_ENOUGH	8125	//炼魂碎片不足
#define MLS_UPGRADE_CHAINSOUL_POS_FAIL		8126	//升级炼魂部件失败

//add by hxw 20151014
#define MLS_QLV_RANK_REWARD_NULL			8127	//无等级排行奖励领取
#define MLS_QINUM_RANK_REWARD_NULL			8128	//无副本挑战排行奖励领取
#define MLS_QINUM_INFO_RANK_REWARD_FAIL		8129	//查询副本挑战次数排行信息错误
#define MLS_QCP_REWARD_FAIL					8130	//查询强者奖励失败
#define MLS_GET_CP_REWARD_FAIL				8131	//获取强者奖励失败
#define MLS_GET_RANK_REWARD_CONFIG_FAIL		8132	//获取通用排行奖励物品配置错误
//end

#define MLS_NOT_OPEN_WORLD_BOSS				8133	//世界Boss未开启
#define MLS_NOT_OPEN_GUARD_ICON				8134	//守卫圣像未开启
#define MLS_INVALID_LADDER_RANK				8135	//无效的天梯排名
#define MLS_LADDER_RANK_LOCKED				8136	//天梯排名已被锁定
#define MLS_DONT_CHALLENGE_SELF				8137	//不能挑战自己
#define MLS_FINISH_CHALLENGE_LADDER_FAIL	8138	//完成挑战天梯失败
#define MLS_FINISH_CHALLENGE_LADDER_TIMEOUT 8139	//完成挑战天梯超时
#define MLS_LADDER_CHALLENGE_COOLING		8140	//天梯挑战正在冷却中
#define MLS_LADDER_CHALLENGE_UPPER_LIMIT	8141	//天梯挑战次数上限
#define MLS_BACKPACK_UPPER_LIMIT			8142	//背包格子上限
#define MLS_WAREHOUSE_UPPER_LIMIT			8143	//仓库格子上限
#define MLS_EXIST_SAME_JOB					8144	//存在相同的职业
#define MLS_NOT_HAS_ACTIVITY_INST_TYPE		8145	//不拥有的活动副本类型
#define MLS_ENTER_COMPMODE_UPPER			8146	//竞技模式进入上限
#define MLS_BUY_ACHIEVE_UPPER				8147	//购买达到上限
#define MLS_VIP_BUY_ACHIEVE_UPPER			8148	//VIP购买达到上限
#define MLS_ENTER_ONE_VS_ONE_UPPER			8149	//1v1模式进入上限
#define MLS_PROPERTY_NOT_ENOUGH				8150	//物品数量不足

//add by hxw 20151020
#define MLS_QONLINE_REWARD_FAIL				8201	//获取在线奖励信息错误
#define MLS_GONLINE_REWARD_FAIL				8202	//获取在线奖励错误
#define MLS_REWARD_ISGET					8203	//已经领取过该奖励
//end
//add by hxw 20151028
#define MLS_QLV_INFO_RANK_REWARD_FAIL		8204	//查询等级排行榜信息错误
//end
//add by hxw 20151029
#define MLS_REWARD_CANNOT_GET				8205	//未达到符合领取条件
//end
//add by hxw 20151117
#define MLS_CASH_SAVE_TYPE_ERROR			8206	//配置文件中的代币存储类型错误
//end

/**************************************************************************
ExtServer(用户管理服务器)错误码定义
***************************************************************************/
#define EXT_OK							0       //成功
#define EXT_ERROR						1       //未知错误
#define EXT_CONFIG_INIT					9001    //初始化配置文件失败
#define EXT_ACCOUNT						9002    //账户信息错误
#define EXT_COMAND						9003    //命令失败
#define EXT_LOGIN_REPEATED				9004    //用户重复登录
#define EXT_NOT_LOGIN					9005    //用户未登录

/*******************************************************************************
***********************************管理服务器错误码*****************************
*******************************************<br class="Apple-interchange-newline">*************************************/
#define		GM_OK										0			//OK
#define		GM_ERROR									1			//其它错误
#define		GM_INVALID_COMMAND							10000		//不能合法的命令码
#define		GM_INVALID_SOCKET_CONN_LINKER				10001		//连接Linker Socket不可用
#define		GM_READ_LINKER_DATA_TIMEOUT					10002		//接收linker数据失败
#define		GM_WRITE_LINKER_DATA_TIMEOUT				10003		//发送linker数据失败
#define		GM_PARSE_KEY_FAILED							10004		//解析Linker发送的加密key失败
#define		GM_QUERY_ACCOUNT_LK_FAILED					10005		//查询帐号留存失败
#define		GM_QUERY_DEVICE_LK_FAILED					10006		//查询设备留存失败
#define		GM_QUERY_ACTOR_LK_FAILED					10007		//查询角色留存失败
#define		GM_INVALID_PARAM							10008		//无效的参数
#define		GM_UNKNOWN_LKTYPE							10009		//不能识别的留存类型
#define		GM_LK_STORE_DB_FALIED						10010		//留存数据写入数据库失败
#define		GM_ONLINE_STORE_DB_FALIED					10011		//在线数据写入数据库失败
#define		GM_AVERAGE_ONLINE_FALIED					10012		//获取平均在线人数失败
#define		GM_INVALID_LKTIME_TYPE						10013		//无效的留存时间类型
#define		GM_AVERAGE_ONLINE_TIME_FAILED				10014		//获取平均在线时间失败
#define		GM_NOT_EXISTS_SESSION						10015		//不存在的会话
#define		GM_USER_LOGIN_FAILED						10016		//登录验证失败
#define		GM_ACCOUNT_INVALID							10017		//无效的帐号信息
#define		GM_GET_ACTIVE_DEVICE_FAILED					10018		//获取激活设备数失败
#define		GM_GET_ACCOUNT_CREATE_FAILED				10019		//获取帐号创建数失败
#define		GM_GET_LOGIN_USER_FAILED					10020		//获取登陆用户数失败
#define		GM_GET_ADD_NEW_FAILED						10021		//获取新增用户数失败
#define		GM_GET_ACTIVE_ACTOR_FAILED					10022		//获取活跃户数失败
#define		GM_GET_ADDUP_NEW_FAILED						10023		//获取累计新增失败
#define		GM_GET_AVERAGE_TIME_FAILED					10024		//获取平均在线时间失败
#define		GM_GET_AVERAGE_PLAYGAME_FAILED				10025		//获取平均游戏次数失败
#define		GM_GET_ONLINE_FAILED						10026		//获取实时在线失败;
#define		GM_GET_MAX_ONLINE_FAILED					10027		//获取最大在线人数失败
#define		GM_GET_AVERAGE_ONLINE_FAILED				10028		//获取平均在线人数失败
#define		GM_GET_PAYMENT_ACTOR_FAILED					10029		//获取付费用户数失败
#define		GM_GET_PAYMENT_COUNT_FAILED					10030		//获取付费用次数
#define		GM_GET_FIRST_RECHARGE_COUNT_FAILED			10031		//获取新增用户首充次数失败
#define		GM_GET_ADDUP_NEW_PAYMENT_FAILED				10032		//获取累计新增付费用户数失败
#define		GM_GET_ADD_NEW_PAYMENT_MONEY_FAILED			10033		//获取新增付费金额失败
#define		GM_GET_PAYMENT_MONEY_FAILED					10034		//获取付费金额失败
#define		GM_TOTAL_PAYMENT_FAILED						10035		//获取累计付费金额失败
#define		GM_ACTIVATED_DEVICE_FAILED					10036		//获取激活设备失败
#define		GM_COST_TOTAL_FAILED						10037		//获取消费总金额失败
#define		GM_SINGLE_ACTOR_COST_TOTAL_FAILED			10038		//获取单个角色消费总金额失败
#define		GM_ACCOUNT_DETAIL_FAILED					10039		//获取帐号详情失败
#define		GM_ACTOR_DETAIL_FAILED						10040		//获取角色详情失败
#define		GM_CONN_SERVER_FAILED						10041		//连接服务器失败
#define     GM_QUERY_NEWSTATISTICS_FAILED				10042	//获取新增统计中有失败;
#define		GM_RECHARGE_STATISTICS_FAILED				10043	//获取查询订单充值详情失败
#define		GM_RECHARGE_STATISTICS_NUM					10044	//获取充值数据统计失败;
#define		GM_QUERY_ONLINENUM_STATISTICS_FAILED		10045	//获取在线数据统计失败;
#define		GM_QUERY_ONLINENUM_NUM_FAILED				10046	//获取在线人数失败;
#define		GM_QUERY_LEVEL_DISTRIBUTION_FAILED			10047	//获取等级分布失败;
#define     GM_QUERY_TIME								10048	//时间错误;

/**************************************************************************
mauServer(变色龙服务器)错误码定义
***************************************************************************/
#define MAU_OK							0       //成功
#define MAU_ERROR						1       //未知错误
#define MAU_GWS_LOGIN_FAILED           11001    //登录网关服务器失败
#define MAU_DBS_LOGIN_FAILED           11002    //登录数据服务器失败
#define MAU_ID_EXIST_IN_CHANNEL        11003    //id在频道内已存在
#define MAU_ID_NOT_EXIST_IN_CHANNEL    11004    //id在牌桌内不存在
#define MAU_TABLE_MEMBERS_EMPTY			11005    //该牌桌成员为空
#define MAU_TABLE_NOT_ALLOW_EL			11006    //该牌桌不允许进入或离开
#define MAU_INVALID_CMD_PACKET         11007    //非法的命令数据包
#define MAU_UNKNOWN_CMD_CODE           11008    //未知的命令码

#endif