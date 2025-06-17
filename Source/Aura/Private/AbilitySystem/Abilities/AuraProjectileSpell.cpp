// Copyright Ivy


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
//#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility(C++)"), true, true, FLinearColor::Yellow, 5.f);

	
	
	

	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& CombatSocket, bool bOverridePitch, float PitchOverride)
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
		FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), CombatSocket);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}


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

		Projectile->DamageEffectParams = MakeDamgeEffectParamsFormClassDefaults();//这里的TargetActor暂时设为nullptr因为这里只是生成逻辑， 
		                                                                                  //TargetActor应该在EffectActor的BeginOverlap再次设置。

		Projectile->FinishSpawning(SpawnTransform);
	}
}
