#include "MLogicFactory.h"
#include "ServiceCmdCode.h"
#include "cyaLog.h"
#include "cyaTcp.h"
#include "MLogicProcInc.h"

DEFINE_FETCH_OBJ(ProcessFactory)

ProcessFactory::ProcessFactory()
{

}

ProcessFactory::~ProcessFactory()
{

}

// 仅LinkServer向GateWayServer转发给MLogicServer消息时需要处理(消息添加CmdCode/UserID/RoleID等信息)
// 默认分支返回RoseTcp_New(MLogicCommonEx)
IMLogicClient* ProcessFactory::Instance(UINT16 cmdCode)
{
	IMLogicClient* pClient = NULL;
	switch (cmdCode)
	{
	case MLS_QUERY_GUILD_NAME:      //10048, 查询公会名称
	case LICENCE_QUERY_UNLOCK_JOBS: //10135, 查询已解锁内容
		// 消息数据最前面插入CmdCode字段
		pClient = CyaTcp_New(MLogicCommonCmd);
		break;

	case MLS_QUERY_ACCEPTED_TASK:   //10020, 查询已接受任务
	case MLS_QUERY_ROLE_SKILLS:     //10055, 查询角色已有技能
		//  case MLS_QUERY_ROLE_COMBOS:     //10056, 查询角色已有连招
	case MLS_GET_DAILY_TASK:        //10065, 获取日常任务
	case MLS_GET_ROLE_OCCUPY_INST:  //10081, 获取角色占领副本
	case MLS_QUERY_ROLE_DRESS:      //10085, 查询角色拥有时装
	case MLS_QUERY_EQUIP_FRAGMENT:  //10095, 查询装备碎片
	case MLS_GET_OPEN_PERSONAL_RAND_INST:   //10098, 获取已打开单人随机副本
	case MLS_QUERY_GOLD_EXCHANGE:   //10100, 查询当天金币兑换次数
	case MLS_GOLD_EXCHANGE:         //10101, 金币兑换
	case MLS_GET_RECOMMEND_FRIEND_LIST: //10106, 获取推荐好友列表
	case MLS_GET_ROULETTE_REMAIN_REWARD: //10137 获取轮盘剩余奖品
	case MLS_EXTRACT_ROULETTE_REWARD: //10138 抽取轮盘奖品
	case MLS_UPDATE_ROULETTE_BIG_REWARD://10141, 刷新轮盘大奖
	case MLS_FETCH_ROULETTE_BIG_REWARD: //10142, 获取轮盘大奖
	case MLS_GET_ACTIVE_INST_CHALLENGE_TIMES://10143, 获取活动副本当天挑战次数
	case MLS_QUERY_VIT_EXCHANGE:    //10145, 查询当天体力兑换次数
	case MLS_VIT_EXCHANGE:          //10146, 体力兑换
	case MLS_QUERY_ROLE_DAILY_SIGN: //10147, 查询角色每日签到
	case MLS_ROLE_DAILY_SIGN:       //10148, 角色每日签到
	case MLS_QUERY_CHAIN_SOUL_FRAGMENT: //10150, 查询炼魂碎片
	case MLS_QUERY_CHAIN_SOUL_POS:  //10151, 查询炼魂部件
		// 消息数据有且仅有CmdCode&&UserID&&RoleID字段
		pClient = CyaTcp_New(MLogicCommon);
		break;

	case MLS_QUERY_BACKPACK:        //10002, 查询背包
	case MLS_QUERY_EMAIL_CONTENT:   //10004, 查询邮件内容
	case MLS_QUERY_FRIEND:          //10006, 查询好友
	case MLS_ADD_FRIEND:            //10007, 添加好友
	case MLS_DEL_FRIEND:            //10008, 删除好友
	case MLS_QUERY_FINISHED_HELP:   //10009, 查询已完成新手引导信息
	case MLS_SAVE_ROLE_ATTR:        //10013, 保存属性点
	case MLS_FINISH_ROLE_HELP:      //10014, 完成新手引导
	case MLS_QUERY_ROLE_APPEARANCE: //10017, 查询角色外型信息
	case MLS_QUERY_ROLE_EXPLICIT:   //10018, 查询角色详细信息
	case MLS_DELTA_TOKEN:           //10021, 充值代币
	case MLS_SELL_PROPERTY:         //10023, 出售道具
	case MLS_DROP_PROPERTY:         //10025, 丢弃道具
	case MLS_CASH_PROPERTY:         //10026, 兑换道具
	case MLS_DROP_TASK:             //10027, 放弃任务
	case MLS_PICKUP_INST_BOX:       //10029, 拾取副本宝箱
	case MLS_BUY_STORAGE_SPACE:     //10030, 购买背包或仓库储存空间
	case MLS_ACCEPT_TASK:           //10031, 接受任务
	case MLS_ENTER_INST:            //10032, 进入副本
		//  case MLS_RECEIVE_REWARD:        //10033, 领取成就奖励
	case MLS_USE_PROPERTY:          //10035, 使用道具
	case MLS_RECEIVE_EMAIL_ATTACHMENT: //10036, 收取邮件附件
	case MLS_FINISH_INST:           //10037, 完成某副本
	case MLS_FETCH_ROBOT:           //10038, 获取机器人数据
		//  case MLS_INLAID_GEM:            //10039, 镶嵌宝石
	case MLS_QUERY_EMAIL_LIST:      //10041, 邮件查询列表
	case MLS_LOAD_EQUIPMENT:        //10043, 载入装备
	case MLS_UNLOAD_EQUIPMENT:      //10044, 卸载装备
		//  case MLS_REMOVE_GEM:            //10047, 取出宝石
	case MLS_PERSONAL_TIMER_TASK:   //10051, 个人定时任务请求
	case MLS_FETCH_INST_AREA_DROPS: //10053, 获取副本区域怪掉落
	case MLS_SKILL_UPGRADE:         //10056, 技能升级
	case MLS_CHANGE_FACTION:        //10057, 改变阵营
	case MLS_LOAD_SKILL:            //10058, 装载技能
	case MLS_CANCEL_SKILL:          //10059, 取消技能
	case MLS_UPDATE_TASK_TARGET:    //10061, 更新已接任务目标
	case MLS_COMMIT_TASK_FINISH:    //10062, 提交任务完成
	case MLS_QUERY_FIN_TASK_HISTORY://10064, 查询已完成任务历史记录
		//  case MLS_STRENGTHEN_EQUIPMENT:  //10066, 强化装备
		//  case MLS_EQUIPMENT_RISE_STAR:   //10067, 装备升星
	case MLS_EQUIPMENT_COMPOSE:     //10068, 装备合成
		//  case MLS_EQUIPMENT_CLEAR_STAR:  //10069, 装备消星
		//  case MLS_EQUIPMENT_INHERIT:     //10070, 装备继承
		//  case MLS_EQUIPMENT_STRENGTHEN_CLEAR://10072, 装备强化消除
	case MLS_GEM_COMPOSE:           //10073, 宝石合成
	case MLS_PROP_BUY_BACK:         //10076, 物品回购
	case MLS_QUERY_ROLE_SELL_PROP_RCD:  //10077, 查询角色出售物品记录
	case MLS_SET_ROLE_TITLE_ID:     //10082, 设置角色职业称号id
	case MLS_DEL_EMAIL:             //10083, 删除角色邮件
	case MLS_MALL_BUY_GOODS:        //10084, 商城购买物品
	case MLS_WEAR_DRESS:            //10086, 穿戴时装
	case MLS_PICKUP_INST_MONSTER_DROP:      //10087, 拾取副本怪掉落物品
	case MLS_GET_PERSONAL_TRANSFER_ENTER:   //10094, 获取单人随机副本传送阵进入人数
		//  case MLS_EQUIP_FRAGMENT_COMPOSE:        //10096, 装备碎片合成
	case MLS_ROLE_REVIVE:           //10097, 角色复活
	case MLS_FRIEND_APPLY_CONFIRM:  //10103, 好友申请确认
	case MLS_QUERY_FRIEND_APPLY:    //10105, 查询好友申请
	case MLS_SEARCH_PLAYER:         //10107, 搜索玩家
	case MLS_NOT_RECEIVE_VIP_REWARD://10112, 未领取奖励的VIP等级
	case MLS_RECEIVE_VIP_REWARD:    //10113, 领取VIP奖励
	case MLS_QUERY_FIRST_RECHARGE_GIVE: //10114, 查询已充值且有首充赠送的充值id
	case MLS_KILL_MONSTER_EXP:      //10115, 杀怪得经验
	case MLS_RECEIVE_TASK_REWARD:   //10116, 领取任务奖励(交任务)
	case MLS_PICKUP_BROKEN_DROP:    //10118, 拾取破碎物掉落
	case MLS_QUERY_MAGICS:          //10119, 查询角色拥有魔导器
	case MLS_GAIN_MAGIC:            //10120, 获得魔导器
	case MLS_EQUIP_POS_STRENGTHEN:  //10123, 装备位强化
	case MLS_EQUIP_POS_RISE_STAR:   //10124, 装备位升星
	case MLS_EQUIP_POS_INLAID_GEM:  //10125, 装备位镶嵌宝石
	case MLS_EQUIP_POS_REMOVE_GEM:  //10126, 装备位取出宝石
	case MLS_GET_EQUIP_POS_ATTR:    //10127, 获取装备位属性
	case MLS_QUERY_ONLINE_SIGN_IN:  //10128, 查询当前签到
	case MLS_ONLINE_SIGN_IN:        //10129, 上线签到
	case MLS_QUERY_RECEIVED_ACTIVITY: //10130, 查询已领取活动
	case MLS_RECEIVED_ACTIVITY:     //10131, 领取活动
	case MLS_QUERY_CUR_OPEN_CHAPTER://10132, 查询当前开放章节
	case MLS_INST_SWEEP:            //10133, 副本扫荡
	case MLS_RECEIVE_CASH_CODE_REWARD://10134, 领取兑换码奖励
	case MLS_QUERY_UNLOCK_CONTENT:  //10135, 查询已解锁内容
	case MLS_CONTENT_UNLOCK:        //10136, 内容解锁
	case MLS_GET_RECHARGE_TOKEN:    //10140 获取充值代币
		// 消息数据最前面插入CmdCode&&UserID&&RoleID字段
		pClient = CyaTcp_New(MLogicCommonEx);
		break;
	case MLS_FETCH_RECHARGE_ORDER: //10139 获取充值订单
		pClient = CyaTcp_New(MLogicCommonExPart);
		break;
		//  case MLS_QUERY_ACHIEVEMENT:     //10003, 查询成就
	case MLS_QUERY_FINISHED_INST:   //10010, 查询已完成副本信息
		pClient = CyaTcp_New(MLogicCommonRole);
		break;

	case MLS_CREATE_ROLE:           //10005, 创建角色
	case MLS_DEL_ROLE:              //10046, 删除角色
		pClient = CyaTcp_New(MLogicCommonUser);
		break;
	case MLS_QUERY_ROLES:           //10000, 查询角色信息
		pClient = CyaTcp_New(MLogicQueryRole);
		break;
	case MLS_ENTER_ROLE:            //10001, 进入角色
		pClient = CyaTcp_New(MLogicEnterRole);
		break;
	case MLS_LEAVE_ROLE:            //10045, 退出角色
		pClient = CyaTcp_New(MLogicRoleExit);
		break;
	case MLS_USER_EXIT:             //10049, 账号退出
		pClient = CyaTcp_New(MLogicUserExit);
		break;

		/*case MLS_BUFF_OVER_NOTIFY:      //10011, buf结束通知
		case MLS_SAVE_GEAR_DESTROY_STATE://10012, 保存机关破坏状态
		case MLS_BACKPACK_MOD_NOTIFY:   //10015, 背包更改主动通知
		case MLS_QUERY_CUR_CASH:        //10016, 查询当前兑换表
		case MLS_QUERY_GIFT_BAG:        //10019, 查询礼品包
		case MLS_DELTA_NOTIFY:          //10022, 充值通知
		case MLS_FLUSH_CASH:            //10024, 刷新兑换表
		case MLS_NEW_EMAIL_NOTIFY:      //10028, 服务器主动通知有新邮件
		case MLS_UPDATE_NOTIFY:         //10034, 升级通知
		case MLS_MOD_ROLE_ATTR:         //10042, 重置属性点
		case MLS_PERSONAL_TIMER_TASK_FIN:/// 10052, 个人定时任务完成
		case MLS_ROLE_UPGRADE_NOTIFY:   //10054, 角色升级通知
		case MLS_GAIN_MAGIC_NOTIFY:     //10121, 获得魔导器通知
		case MLS_FRAGMENT_UPDATE_NOTIFY://10122, 装备碎片刷新通知
		{
		#pragma compiling_remind("客户端不需要发这个请求，服务器会主动推送")
		pClient = NULL;
		}
		default:
		LogDebug(("错误的对主逻辑服务器命令请求"));
		ASSERT(FALSE);
		*/

	default:
		// 消息数据最前面插入CmdCode&&UserID&&RoleID字段
		pClient = CyaTcp_New(MLogicCommonEx);
		break;
	}
#pragma compiling_remind("新的对主逻辑消息请求类型在这添加")
	return pClient;
}

void ProcessFactory::DeleteInstance(IMLogicClient* pObj)
{
	CyaTcp_Delete(pObj);
}

