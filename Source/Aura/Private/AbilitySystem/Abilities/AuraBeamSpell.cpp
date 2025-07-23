// Copyright Ivy


#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/Character.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

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

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
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

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*>ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			UKismetSystemLibrary::SphereTraceSingle
			(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				10.f,
				TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true
			);
			if (HitResult.bBlockingHit)
			{
				MouseHitActor = HitResult.GetActor();
				MouseHitLocation = HitResult.ImpactPoint;
			}
		}
	}
	if (ICombatInterface*CombatInterface=Cast<ICombatInterface>(MouseHitActor))
	{
		CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*>ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	TArray<AActor*>OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(GetAvatarActorFromActorInfo(), OverlappingActors, ActorsToIgnore, 850.f, MouseHitActor->GetActorLocation());

	//int32 NumAdditionTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	int32 NumAdditionTargets = 5.f;

	UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());

	for (AActor*Target: OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			// ���Ƴ��������ظ��󶨣��������°�
			CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
		}
	}
	
}
