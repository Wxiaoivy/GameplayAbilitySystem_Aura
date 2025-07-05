// Copyright Ivy


#include "AbilitySystem/Abilities/AuraFireBolt.h"

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
	const float ManaCost = GetManaCost(Level);
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
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-ProjectileSpread / 2.f, FVector::UpVector);
	const FVector RightOfSpread = Forward.RotateAngleAxis(ProjectileSpread / 2.f, FVector::UpVector);

	//NumProjectiles = FMath::Max(MaxNumProjectiles, GetAbilityLevel());
	const float DeltaSpread = ProjectileSpread / (NumProjectiles - 1);
	if (NumProjectiles > 1)
	{
		for (int32 i = 0; i < NumProjectiles; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			const FVector Start = SocketLocation + FVector(0, 0, 10);
			UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Start, Start + Direction * 100.f, 1.f, FLinearColor::Red, 120.f, 1.f);
		}
	} 
	else
	{
		const FVector Start = SocketLocation + FVector(0, 0, 10);
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Start, Start + Forward	 * 100.f, 1.f, FLinearColor::Red, 120.f, 1.f);
	}
}

