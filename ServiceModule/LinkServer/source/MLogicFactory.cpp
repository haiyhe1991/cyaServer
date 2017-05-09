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

// ��LinkServer��GateWayServerת����MLogicServer��Ϣʱ��Ҫ����(��Ϣ���CmdCode/UserID/RoleID����Ϣ)
// Ĭ�Ϸ�֧����RoseTcp_New(MLogicCommonEx)
IMLogicClient* ProcessFactory::Instance(UINT16 cmdCode)
{
	IMLogicClient* pClient = NULL;
	switch (cmdCode)
	{
	case MLS_QUERY_GUILD_NAME:      //10048, ��ѯ��������
	case LICENCE_QUERY_UNLOCK_JOBS: //10135, ��ѯ�ѽ�������
		// ��Ϣ������ǰ�����CmdCode�ֶ�
		pClient = CyaTcp_New(MLogicCommonCmd);
		break;

	case MLS_QUERY_ACCEPTED_TASK:   //10020, ��ѯ�ѽ�������
	case MLS_QUERY_ROLE_SKILLS:     //10055, ��ѯ��ɫ���м���
		//  case MLS_QUERY_ROLE_COMBOS:     //10056, ��ѯ��ɫ��������
	case MLS_GET_DAILY_TASK:        //10065, ��ȡ�ճ�����
	case MLS_GET_ROLE_OCCUPY_INST:  //10081, ��ȡ��ɫռ�츱��
	case MLS_QUERY_ROLE_DRESS:      //10085, ��ѯ��ɫӵ��ʱװ
	case MLS_QUERY_EQUIP_FRAGMENT:  //10095, ��ѯװ����Ƭ
	case MLS_GET_OPEN_PERSONAL_RAND_INST:   //10098, ��ȡ�Ѵ򿪵����������
	case MLS_QUERY_GOLD_EXCHANGE:   //10100, ��ѯ�����Ҷһ�����
	case MLS_GOLD_EXCHANGE:         //10101, ��Ҷһ�
	case MLS_GET_RECOMMEND_FRIEND_LIST: //10106, ��ȡ�Ƽ������б�
	case MLS_GET_ROULETTE_REMAIN_REWARD: //10137 ��ȡ����ʣ�ཱƷ
	case MLS_EXTRACT_ROULETTE_REWARD: //10138 ��ȡ���̽�Ʒ
	case MLS_UPDATE_ROULETTE_BIG_REWARD://10141, ˢ�����̴�
	case MLS_FETCH_ROULETTE_BIG_REWARD: //10142, ��ȡ���̴�
	case MLS_GET_ACTIVE_INST_CHALLENGE_TIMES://10143, ��ȡ�����������ս����
	case MLS_QUERY_VIT_EXCHANGE:    //10145, ��ѯ���������һ�����
	case MLS_VIT_EXCHANGE:          //10146, �����һ�
	case MLS_QUERY_ROLE_DAILY_SIGN: //10147, ��ѯ��ɫÿ��ǩ��
	case MLS_ROLE_DAILY_SIGN:       //10148, ��ɫÿ��ǩ��
	case MLS_QUERY_CHAIN_SOUL_FRAGMENT: //10150, ��ѯ������Ƭ
	case MLS_QUERY_CHAIN_SOUL_POS:  //10151, ��ѯ���겿��
		// ��Ϣ�������ҽ���CmdCode&&UserID&&RoleID�ֶ�
		pClient = CyaTcp_New(MLogicCommon);
		break;

	case MLS_QUERY_BACKPACK:        //10002, ��ѯ����
	case MLS_QUERY_EMAIL_CONTENT:   //10004, ��ѯ�ʼ�����
	case MLS_QUERY_FRIEND:          //10006, ��ѯ����
	case MLS_ADD_FRIEND:            //10007, ��Ӻ���
	case MLS_DEL_FRIEND:            //10008, ɾ������
	case MLS_QUERY_FINISHED_HELP:   //10009, ��ѯ���������������Ϣ
	case MLS_SAVE_ROLE_ATTR:        //10013, �������Ե�
	case MLS_FINISH_ROLE_HELP:      //10014, �����������
	case MLS_QUERY_ROLE_APPEARANCE: //10017, ��ѯ��ɫ������Ϣ
	case MLS_QUERY_ROLE_EXPLICIT:   //10018, ��ѯ��ɫ��ϸ��Ϣ
	case MLS_DELTA_TOKEN:           //10021, ��ֵ����
	case MLS_SELL_PROPERTY:         //10023, ���۵���
	case MLS_DROP_PROPERTY:         //10025, ��������
	case MLS_CASH_PROPERTY:         //10026, �һ�����
	case MLS_DROP_TASK:             //10027, ��������
	case MLS_PICKUP_INST_BOX:       //10029, ʰȡ��������
	case MLS_BUY_STORAGE_SPACE:     //10030, ���򱳰���ֿⴢ��ռ�
	case MLS_ACCEPT_TASK:           //10031, ��������
	case MLS_ENTER_INST:            //10032, ���븱��
		//  case MLS_RECEIVE_REWARD:        //10033, ��ȡ�ɾͽ���
	case MLS_USE_PROPERTY:          //10035, ʹ�õ���
	case MLS_RECEIVE_EMAIL_ATTACHMENT: //10036, ��ȡ�ʼ�����
	case MLS_FINISH_INST:           //10037, ���ĳ����
	case MLS_FETCH_ROBOT:           //10038, ��ȡ����������
		//  case MLS_INLAID_GEM:            //10039, ��Ƕ��ʯ
	case MLS_QUERY_EMAIL_LIST:      //10041, �ʼ���ѯ�б�
	case MLS_LOAD_EQUIPMENT:        //10043, ����װ��
	case MLS_UNLOAD_EQUIPMENT:      //10044, ж��װ��
		//  case MLS_REMOVE_GEM:            //10047, ȡ����ʯ
	case MLS_PERSONAL_TIMER_TASK:   //10051, ���˶�ʱ��������
	case MLS_FETCH_INST_AREA_DROPS: //10053, ��ȡ��������ֵ���
	case MLS_SKILL_UPGRADE:         //10056, ��������
	case MLS_CHANGE_FACTION:        //10057, �ı���Ӫ
	case MLS_LOAD_SKILL:            //10058, װ�ؼ���
	case MLS_CANCEL_SKILL:          //10059, ȡ������
	case MLS_UPDATE_TASK_TARGET:    //10061, �����ѽ�����Ŀ��
	case MLS_COMMIT_TASK_FINISH:    //10062, �ύ�������
	case MLS_QUERY_FIN_TASK_HISTORY://10064, ��ѯ�����������ʷ��¼
		//  case MLS_STRENGTHEN_EQUIPMENT:  //10066, ǿ��װ��
		//  case MLS_EQUIPMENT_RISE_STAR:   //10067, װ������
	case MLS_EQUIPMENT_COMPOSE:     //10068, װ���ϳ�
		//  case MLS_EQUIPMENT_CLEAR_STAR:  //10069, װ������
		//  case MLS_EQUIPMENT_INHERIT:     //10070, װ���̳�
		//  case MLS_EQUIPMENT_STRENGTHEN_CLEAR://10072, װ��ǿ������
	case MLS_GEM_COMPOSE:           //10073, ��ʯ�ϳ�
	case MLS_PROP_BUY_BACK:         //10076, ��Ʒ�ع�
	case MLS_QUERY_ROLE_SELL_PROP_RCD:  //10077, ��ѯ��ɫ������Ʒ��¼
	case MLS_SET_ROLE_TITLE_ID:     //10082, ���ý�ɫְҵ�ƺ�id
	case MLS_DEL_EMAIL:             //10083, ɾ����ɫ�ʼ�
	case MLS_MALL_BUY_GOODS:        //10084, �̳ǹ�����Ʒ
	case MLS_WEAR_DRESS:            //10086, ����ʱװ
	case MLS_PICKUP_INST_MONSTER_DROP:      //10087, ʰȡ�����ֵ�����Ʒ
	case MLS_GET_PERSONAL_TRANSFER_ENTER:   //10094, ��ȡ������������������������
		//  case MLS_EQUIP_FRAGMENT_COMPOSE:        //10096, װ����Ƭ�ϳ�
	case MLS_ROLE_REVIVE:           //10097, ��ɫ����
	case MLS_FRIEND_APPLY_CONFIRM:  //10103, ��������ȷ��
	case MLS_QUERY_FRIEND_APPLY:    //10105, ��ѯ��������
	case MLS_SEARCH_PLAYER:         //10107, �������
	case MLS_NOT_RECEIVE_VIP_REWARD://10112, δ��ȡ������VIP�ȼ�
	case MLS_RECEIVE_VIP_REWARD:    //10113, ��ȡVIP����
	case MLS_QUERY_FIRST_RECHARGE_GIVE: //10114, ��ѯ�ѳ�ֵ�����׳����͵ĳ�ֵid
	case MLS_KILL_MONSTER_EXP:      //10115, ɱ�ֵþ���
	case MLS_RECEIVE_TASK_REWARD:   //10116, ��ȡ������(������)
	case MLS_PICKUP_BROKEN_DROP:    //10118, ʰȡ���������
	case MLS_QUERY_MAGICS:          //10119, ��ѯ��ɫӵ��ħ����
	case MLS_GAIN_MAGIC:            //10120, ���ħ����
	case MLS_EQUIP_POS_STRENGTHEN:  //10123, װ��λǿ��
	case MLS_EQUIP_POS_RISE_STAR:   //10124, װ��λ����
	case MLS_EQUIP_POS_INLAID_GEM:  //10125, װ��λ��Ƕ��ʯ
	case MLS_EQUIP_POS_REMOVE_GEM:  //10126, װ��λȡ����ʯ
	case MLS_GET_EQUIP_POS_ATTR:    //10127, ��ȡװ��λ����
	case MLS_QUERY_ONLINE_SIGN_IN:  //10128, ��ѯ��ǰǩ��
	case MLS_ONLINE_SIGN_IN:        //10129, ����ǩ��
	case MLS_QUERY_RECEIVED_ACTIVITY: //10130, ��ѯ����ȡ�
	case MLS_RECEIVED_ACTIVITY:     //10131, ��ȡ�
	case MLS_QUERY_CUR_OPEN_CHAPTER://10132, ��ѯ��ǰ�����½�
	case MLS_INST_SWEEP:            //10133, ����ɨ��
	case MLS_RECEIVE_CASH_CODE_REWARD://10134, ��ȡ�һ��뽱��
	case MLS_QUERY_UNLOCK_CONTENT:  //10135, ��ѯ�ѽ�������
	case MLS_CONTENT_UNLOCK:        //10136, ���ݽ���
	case MLS_GET_RECHARGE_TOKEN:    //10140 ��ȡ��ֵ����
		// ��Ϣ������ǰ�����CmdCode&&UserID&&RoleID�ֶ�
		pClient = CyaTcp_New(MLogicCommonEx);
		break;
	case MLS_FETCH_RECHARGE_ORDER: //10139 ��ȡ��ֵ����
		pClient = CyaTcp_New(MLogicCommonExPart);
		break;
		//  case MLS_QUERY_ACHIEVEMENT:     //10003, ��ѯ�ɾ�
	case MLS_QUERY_FINISHED_INST:   //10010, ��ѯ����ɸ�����Ϣ
		pClient = CyaTcp_New(MLogicCommonRole);
		break;

	case MLS_CREATE_ROLE:           //10005, ������ɫ
	case MLS_DEL_ROLE:              //10046, ɾ����ɫ
		pClient = CyaTcp_New(MLogicCommonUser);
		break;
	case MLS_QUERY_ROLES:           //10000, ��ѯ��ɫ��Ϣ
		pClient = CyaTcp_New(MLogicQueryRole);
		break;
	case MLS_ENTER_ROLE:            //10001, �����ɫ
		pClient = CyaTcp_New(MLogicEnterRole);
		break;
	case MLS_LEAVE_ROLE:            //10045, �˳���ɫ
		pClient = CyaTcp_New(MLogicRoleExit);
		break;
	case MLS_USER_EXIT:             //10049, �˺��˳�
		pClient = CyaTcp_New(MLogicUserExit);
		break;

		/*case MLS_BUFF_OVER_NOTIFY:      //10011, buf����֪ͨ
		case MLS_SAVE_GEAR_DESTROY_STATE://10012, ��������ƻ�״̬
		case MLS_BACKPACK_MOD_NOTIFY:   //10015, ������������֪ͨ
		case MLS_QUERY_CUR_CASH:        //10016, ��ѯ��ǰ�һ���
		case MLS_QUERY_GIFT_BAG:        //10019, ��ѯ��Ʒ��
		case MLS_DELTA_NOTIFY:          //10022, ��ֵ֪ͨ
		case MLS_FLUSH_CASH:            //10024, ˢ�¶һ���
		case MLS_NEW_EMAIL_NOTIFY:      //10028, ����������֪ͨ�����ʼ�
		case MLS_UPDATE_NOTIFY:         //10034, ����֪ͨ
		case MLS_MOD_ROLE_ATTR:         //10042, �������Ե�
		case MLS_PERSONAL_TIMER_TASK_FIN:/// 10052, ���˶�ʱ�������
		case MLS_ROLE_UPGRADE_NOTIFY:   //10054, ��ɫ����֪ͨ
		case MLS_GAIN_MAGIC_NOTIFY:     //10121, ���ħ����֪ͨ
		case MLS_FRAGMENT_UPDATE_NOTIFY://10122, װ����Ƭˢ��֪ͨ
		{
		#pragma compiling_remind("�ͻ��˲���Ҫ��������󣬷���������������")
		pClient = NULL;
		}
		default:
		LogDebug(("����Ķ����߼���������������"));
		ASSERT(FALSE);
		*/

	default:
		// ��Ϣ������ǰ�����CmdCode&&UserID&&RoleID�ֶ�
		pClient = CyaTcp_New(MLogicCommonEx);
		break;
	}
#pragma compiling_remind("�µĶ����߼���Ϣ���������������")
	return pClient;
}

void ProcessFactory::DeleteInstance(IMLogicClient* pObj)
{
	CyaTcp_Delete(pObj);
}

