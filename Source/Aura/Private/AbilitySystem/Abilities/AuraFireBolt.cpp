// Copyright Ivy


#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT
		(

			//Title
			"<Title>FIRE BOLT</>\n\n"

			//Level
			"<Small>Level: </><Level>%d</>\n"
			//ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n"

			"<Default>Launches a bolt of fire,"
			"Exploding on impact and dealing: </>"

			//Damage
			"<Damage>%d</><Default>fire damage with a chance to burn</>"

		),
			//Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage
		);
	}
	return FString::Printf(TEXT
	(

		//Title
		"<Title>FIRE BOLT</>\n\n"

		//Level
		"<Small>Level: </><Level>%d</>\n"
		//ManaCost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		//Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		//Firebolt Num
		"<Default>Launches %d bolt of fire,"
		"Exploding on impact and dealing: </>"

		//Damage
		"<Damage>%d</><Default>fire damage with a chance to burn</>"

	),
		//Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level,SpellNum),
		ScaledDamage
	);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT
	(

		//Title
		"<Title>NEXT LEVEL</>\n\n"

		//Level
		"<Small>Level: </><Level>%d</>\n"
		//ManaCost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		//Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		//Firebolt Num
		"<Default>Launches %d bolt of fire,"
		"Exploding on impact and dealing: </>"

		//Damage
		"<Damage>%d</><Default>fire damage with a chance to burn</>"

	),
		//Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, SpellNum),
		ScaledDamage
	);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& CombatSocket, bool bOverridePitch /*= false*/, float PitchOverride /*= 0.f*/, AActor* HomingTarget)
{
	// 仅在服务器端（Server）生成投射物（Projectile），而在客户端（Client）直接返回，不执行生成逻辑。
	// 这是多人联网游戏中的一种常见设计模式，目的是确保 网络同步的权威性（Authority） 和 避免重复生成。
	const bool bIsSever = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsSever)return;

	//如果GetAvatarActorFromActorInfo()返回的AActor实现了ICombatInterface接口，那么CombatInterface就是有效的；否则，CombatInterface为nullptr。
	//ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), CombatSocket);
	
		/*	Execute_GetCombatSocketLocation 是 静态全局函数，它的作用是：
				接收一个 UObject* 参数（通常是实现了该接口的对象）。
				检查该对象是否实现了 ICombatInterface。
				调用该对象的 GetCombatSocketLocation() 或 _Implementation 版本。*/

	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)Rotation.Pitch = PitchOverride;


	const FVector Forward = Rotation.Vector();
	NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
	TArray<FRotator>Rotations = UAuraAbilitySystemLibrary::EvenlySpaceRotators(Forward, FVector::UpVector, ProjectileSpread, NumProjectiles);
	for (const FRotator&Rot: Rotations)
	{
		FTransform SpawnTransform;//生成抛射物的位置，在ICombatInterface里实现
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

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

		if (HomingTarget&&HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		} 
		else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}

		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;

		Projectile->FinishSpawning(SpawnTransform);
	}

}

