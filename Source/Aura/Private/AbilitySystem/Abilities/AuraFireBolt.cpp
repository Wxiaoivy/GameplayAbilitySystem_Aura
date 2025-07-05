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
	// ���ڷ������ˣ�Server������Ͷ���Projectile�������ڿͻ��ˣ�Client��ֱ�ӷ��أ���ִ�������߼���
	// ���Ƕ���������Ϸ�е�һ�ֳ������ģʽ��Ŀ����ȷ�� ����ͬ����Ȩ���ԣ�Authority�� �� �����ظ����ɡ�
	const bool bIsSever = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsSever)return;

	//���GetAvatarActorFromActorInfo()���ص�AActorʵ����ICombatInterface�ӿڣ���ôCombatInterface������Ч�ģ�����CombatInterfaceΪnullptr��
	//ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), CombatSocket);
	
		/*	Execute_GetCombatSocketLocation �� ��̬ȫ�ֺ��������������ǣ�
				����һ�� UObject* ������ͨ����ʵ���˸ýӿڵĶ��󣩡�
				���ö����Ƿ�ʵ���� ICombatInterface��
				���øö���� GetCombatSocketLocation() �� _Implementation �汾��*/

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

