// Copyright Ivy


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit) // ���������ЧĿ��
	{
		// �洢����λ��
		MouseHitLocation = HitResult.ImpactPoint;

		// �洢���е�Actor
		// HitResult.GetActor()���ص�����ָ��(UObject*)������ֵ��TObjectPtr���Զ�ת��
		// ��ΪMouseHitActor��UPROPERTY()��������ᴴ��һ��ǿ���ã���ֹ��Actor��GC����,��������ָ��
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		// ���û��������ЧĿ�꣬ȡ������
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerPlayerController()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = OwnerPlayerController = CurrentActorInfo->PlayerController.Get();

		// CurrentActorInfo->PlayerController ��һ��TWeakObjectPtr����ָ�룩
		// .Get()����ָ���ȡ��ָ��(UObject*)
		// ��ֵ��OwnerPlayerController��TObjectPtrǿ���ã��ᴴ��һ��ǿ���ù�ϵ
		// ����PlayerController�Ͳ��ᱻGC����

		// �ص������
		// 1. CurrentActorInfo->PlayerController����ָ��(TWeakObjectPtr)
		// 2. .Get()����ת��Ϊ��ָ��
		// 3. ��ֵ��UPROPERTY() TObjectPtr�ᴴ��ǿ����
		// 4. ������ʹԭʼ��ָ��ʧЧ��PlayerController�Ի���Ϊ�����ǿ���ö����ᱻGC����
		//    ��ֹ GC ���յ���Ҫ���þ��Ǳ�������ָ�뵼�µı���

		/*ָ��ת�����̣�

			��ָ��.Get() �� ��ָ�� �� ��ֵ��TObjectPtrǿ����

			�����Ͱ�һ��������ת������ǿ���ã�ȷ�����󲻻ᱻGC����*/

			/*ǿ���õ��������������������� UAuraBeamSpell���󶨣����ܽ�����

				�������ʵ�������٣�ǿ�����Զ��ͷš�

				�������ʵ�����棬ǿ���û���������ֶ�����

				�Ƿ���ǿ����ȡ��������

				���ڳ��� �� ������(TWeakObjectPtr)��

				���ڳ��� �� ǿ����(UPROPERTY() TObjectPtr)������ע���ͷ�ʱ����*/
	
	}
}
