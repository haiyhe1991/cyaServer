#include "GuardIconMan.h"

#include "ServiceErrorCode.h"
#include "GameCfgFileMan.h"


static GuardIconMan* sg_pGuardIconMan = NULL;
void InitGuardIconMan()
{
	ASSERT(sg_pGuardIconMan == NULL);
	sg_pGuardIconMan = new GuardIconMan();
	ASSERT(sg_pGuardIconMan != NULL);
}

GuardIconMan* GetGuardIconMan()
{
	return sg_pGuardIconMan;
}

void DestroyGuardIconMan()
{
	GuardIconMan* pGuardIconMan = sg_pGuardIconMan;
	sg_pGuardIconMan = NULL;
	if (pGuardIconMan != NULL)
		delete pGuardIconMan;
}

GuardIconMan::GuardIconMan()
{

}

GuardIconMan::~GuardIconMan()
{

}

// ��������ʥ�񸱱�
int GuardIconMan::EnterGuardIconManInst(UINT32 userId, UINT32 roleId)
{
	// ��ȡϵͳ��ǰʱ��
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);

	// �Ƿ��ѿ���
	LimitedTimeActivityCfg::SActivityConfig config;
	if (GetGameCfgFileMan()->GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::GUARD_ICON, config))
	{
		if (config.IsOpen)
		{
			int start_minute = config.StartTimeHour * 60 + config.StartTimeMinute;
			int end_minute = config.EndTimeHour * 60 + config.EndTimeHour;
			int current_minute = lcTime.wHour * 60 + lcTime.wMinute;

			// ��������Boss
			if (current_minute >= start_minute && current_minute <= end_minute)
			{
				return MLS_OK;
			}
		}
	}

	return MLS_NOT_OPEN_GUARD_ICON;
}