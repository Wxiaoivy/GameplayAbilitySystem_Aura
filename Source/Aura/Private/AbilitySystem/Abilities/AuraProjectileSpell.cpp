// Copyright Ivy


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
//#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility(C++)"), true, true, FLinearColor::Yellow, 5.f);

	
	
	

	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// 仅在服务器端（Server）生成投射物（Projectile），而在客户端（Client）直接返回，不执行生成逻辑。
	// 这是多人联网游戏中的一种常见设计模式，目的是确保 网络同步的权威性（Authority） 和 避免重复生成。
	const bool bIsSever = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsSever)return;


	//如果GetAvatarActorFromActorInfo()返回的AActor实现了ICombatInterface接口，那么CombatInterface就是有效的；否则，CombatInterface为nullptr。
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		/*	Execute_GetCombatSocketLocation 是 静态全局函数，它的作用是：
				接收一个 UObject* 参数（通常是实现了该接口的对象）。
				检查该对象是否实现了 ICombatInterface。
				调用该对象的 GetCombatSocketLocation() 或 _Implementation 版本。*/
		FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo());
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;


		FTransform SpawnTransform;//生成抛射物的位置，在ICombatInterface里实现
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, // 抛射物的类
			SpawnTransform,// 抛射物的初始变换（位置、旋转、缩放） 
			GetOwningActorFromActorInfo(), // 抛射物的所有者,武器是角色Actor的SkeletonMeshComponent,所有者必须是AActor 所以这里的所有者和引发者都是角色， 
			//即使武器是一个AActor，逻辑上抛射物的行为（如伤害计算、销毁逻辑）仍然应该归属于角色，因为角色是更高层次的逻辑实体。
			Cast<APawn>(GetOwningActorFromActorInfo()), // 抛射物的引发者（Instigator）      
			//(在Gameplay Ability System中，GetOwningActorFromActorInfo()通常用于获取当前技能的施法者（Caster）。)

			ESpawnActorCollisionHandlingMethod::AlwaysSpawn // 碰撞处理方式:表示无论是否有碰撞，都会生成Actor。即使生成位置有其他物体阻挡，抛射物也会被生成。
		);


		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();

		//补充EffectContext的信息
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors; 
		Actors.Add(Projectile); 
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult; 
		HitResult.Location = ProjectileTargetLocation; 
		EffectContextHandle.AddHitResult(HitResult);
		//补充EffectContext的信息

		const FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass,GetAbilityLevel(), EffectContextHandle);



		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

		for (auto& Pair : UAuraDamageGameplayAbility::DamageTypesMap)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(10/*GetAbilityLevel()*/);
			
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Pair.Key, ScaledDamage);//将数值与特定的GameplayTag关联存储
			//SetByCaller 是一种动态数值传递方式，允许在 Gameplay Effect 执行时传入变量值，而不是硬编码在数据资产里。
		    //给某个技能效果（EffectSpec）动态绑定一个伤害值（50），这个伤害值可以在运行时被修改（比如受技能等级、属性加成等影响）。
		    //例如：
		    // 火球术的伤害可能受 法术强度（Spell Power） 影响，可以使用 SetByCaller 方式动态计算最终伤害。
		    // 治疗技能的治疗量可能受 治疗加成（Healing Bonus） 影响，也可以用这种方式动态调整。
		}
		

		


		EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());
		EffectContextHandle.AddSourceObject(this);

		Projectile->DamageEffectSpecHandle = EffectSpecHandle;


		Projectile->FinishSpawning(SpawnTransform);
	}
}
