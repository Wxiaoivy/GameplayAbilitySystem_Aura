// Copyright Ivy


#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/Character.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit) // 如果命中有效目标
	{
		// 存储命中位置
		MouseHitLocation = HitResult.ImpactPoint;

		// 存储命中的Actor
		// HitResult.GetActor()返回的是裸指针(UObject*)，但赋值给TObjectPtr会自动转换
		// 因为MouseHitActor有UPROPERTY()，所以这会创建一个强引用，阻止该Actor被GC回收,避免悬挂指针
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		// 如果没有命中有效目标，取消技能
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
		// CurrentActorInfo->PlayerController 是一个TWeakObjectPtr（弱指针）
		// .Get()从弱指针获取裸指针(UObject*)
		// 赋值给OwnerPlayerController（TObjectPtr强引用）会创建一个强引用关系
		// 这样PlayerController就不会被GC回收

		// 重点解析：
		// 1. CurrentActorInfo->PlayerController是弱指针(TWeakObjectPtr)
		// 2. .Get()将其转换为裸指针
		// 3. 赋值给UPROPERTY() TObjectPtr会创建强引用
		// 4. 这样即使原始弱指针失效，PlayerController仍会因为有这个强引用而不会被GC回收
		//    阻止 GC 回收的主要作用就是避免悬空指针导致的崩溃

		/*指针转换过程：

			弱指针.Get() → 裸指针 → 赋值给TObjectPtr强引用

			这样就把一个弱引用转换成了强引用，确保对象不会被GC回收*/

			/*强引用的生命周期与所属对象（如 UAuraBeamSpell）绑定，技能结束后：

				如果技能实例被销毁，强引用自动释放。

				如果技能实例缓存，强引用会持续，需手动管理。

				是否用强引用取决于需求：

				短期持有 → 弱引用(TWeakObjectPtr)。

				长期持有 → 强引用(UPROPERTY() TObjectPtr)，但需注意释放时机。*/
	
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
			// 先移除（避免重复绑定），再重新绑定
			CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
		}
	}
	
}
