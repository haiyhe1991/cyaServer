#include "FunModuleMan.h"

static FunModuleMan* sg_funModMan = NULL;
void InitFunModuleMan()
{
	ASSERT(sg_funModMan == NULL);
	sg_funModMan = new FunModuleMan();
	ASSERT(sg_funModMan != NULL);
}

void DestroyFunModuleMan()
{
	FunModuleMan* funModMan = sg_funModMan;
	sg_funModMan = NULL;
	if (funModMan)
		delete funModMan;
}

FunModuleMan::FunModuleMan()
{
	//��ʼ��ϵͳ����
	InitSystemNoticeMan();

	//��ʼ����ɫ����,ħ�������ݹ���
	InitPermanenDataMan();

	//��ʼ���û���ɫ��Ϣ
	InitRolesInfoMan();

	//��ʼ������������Ϣ
	InitInstRankInfoMan();

	//��ʼ�����ݿ�ˢ�¹������
	InitRefreshDBMan();

	//��ʼ��������ʷ��¼����
	InitInstFinHistoryMan();

	//��ʼ���ճ��������
	InitDailyTaskMan();

	//��ʼ�����������ʷ��¼����
	InitTaskFinHistoryMan();

	//��ʼ����Ʒ���۹���
	InitPropSellMan();

	//��ʼ���ʼ�����
	InitEmailCleanMan();

	//��ʼ�����ѹ���
	InitFriendMan();

	//��ʼ���������
	InitGuildMan();

	//��ʼ�������˹���
	InitPlayerInfoMan();

	//��ʼ�������ָ�
	InitRecoveryVitMan();

	//��ʼ����ֵ����
	//InitRechargeMan();

	//��ʼ���̳ǹ���
	InitShoppingMallMan();

	//��ʼ������ռ���ʼ�֪ͨ
	//InitInstOccupyNotify();

	//��ʼ�����Ѽ�¼����
	InitConsumeProduceMan();

	//��ʼ�������������
	//InitPersonalRandInstMan();

	//��ʼ����Ҷһ�
	InitGoldExchangeMan();

	//��ʼ��VIP����
	InitVIPMan();

	//��ʼ������ǩ������
	InitOnlineSingnInMan();

	//��ʼ�������
	InitActitvityMan();

	//��ʼ������
	InitUnlockContentMan();

	//��ʼ�������Ϊ����
	InitPlayerActionMan();

	//��ʼ���ⲿ�������
	InitGMLogicProcMan();

	//��ʼ���ɾ͹���
	InitAchievementMan();

	//��ʼ�������һ�
	InitVitExchangeMan();

	//��ʼ������Boss
	InitWorldBossMan();

	//��ʼ����������
	InitLadderRankMan();

	//��ʼ������ʥ��
	InitGuardIconMan();

	//��ʼ�������
	InitActitvityInstMan();

	//add by hxw 20151020
	//��ʼ�����н����
	InitRankRewardMan();
	//end;

	//��ʼ������ģʽ
	InitCompetitiveModeMan();

	//��ʼ��1v1ģʽ
	InitOneVsOneMan();

	//��ʼ����Ϣ��ʾ
	InitMessageTipsMan();
}

FunModuleMan::~FunModuleMan()
{
	//�ͷ��ⲿ�������
	DestroyGMLogicProcMan();

	//�ͷ������Ϊ����
	DestroyPlayerActionMan();

	//�ͷŽ�������
	DestroyUnlockContentMan();

	//��������
	DestroyActitvityMan();

	//��������ǩ������
	DestroyOnlineSingnInMan();

	//�ͷ�VIP����
	DestroyVIPMan();

	//�ͷŻ���Ҷһ�
	DestroyGoldExchangeMan();

	//�ͷŵ��������������
	//DestroyPersonalRandInstMan();

	//�ͷ����Ѽ�¼����
	DestroyConsumeProduceMan();

	//�ͷŸ���ռ���ʼ�֪ͨ
	//DestroyInstOccupyNotify();

	//�ͷ��̳ǹ���
	DestroyShoppingMallMan();

	//�ͷų�ֵ����
	//DestroyRechargeMan();

	//�ͷ������ָ�
	DestroyRecoveryVitMan();

	//�ͷŻ����˹���
	DestroyPlayerInfoMan();

	//�ͷŹ������
	DestroyGuildMan();

	//�ͷź��ѹ���
	DestroyFriendMan();

	//�ͷ��ʼ�����
	DestroyEmailCleanMan();

	//������Ʒ���۹���
	DestroyPropSellMan();

	//�������������ʷ��¼����
	DestroyTaskFinHistoryMan();

	//�����ճ��������
	DestroyDailyTaskMan();

	//���ٸ�����ʷ��¼����
	DestroyInstFinHistoryMan();

	//�������ݿ�ˢ�¹������
	DestroyRefreshDBMan();

	//���ٸ���������Ϣ
	DestroyInstRankInfoMan();

	//�ͷ��û���ɫ��Ϣ��Դ
	DestroyRolesInfoMan();

	//�ͷŽ�ɫ����,ħ�������ݹ���
	DestroyPermanenDataMan();

	//����ϵͳ����
	DestroySystemNoticeMan();

	//���ٳɾ͹���
	DestroyAchievementMan();

	//��������Boss
	DestroyWorldBossMan();

	//������������
	DestroyLadderRankMan();

	//��������ʥ��
	DestroyGuardIconMan();

	//���ٻ����
	DestroyActitvityInstMan();

	//add by hxw 20151020
	//�������н����ģ��
	DestroyRankRewardMan();
	//end

	//���پ���ģʽ
	DestroyCompetitiveModeMan();

	//����1v1ģʽ
	DestroyOneVsOneMan();

	//������Ϣ��ʾ
	DestroyMessageTipsMan();
}