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
	NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
	TArray<FRotator>Rotations = UAuraAbilitySystemLibrary::EvenlySpaceRotators(Forward, FVector::UpVector, ProjectileSpread, NumProjectiles);
	for (const FRotator&Rot: Rotations)
	{
		FTransform SpawnTransform;//�����������λ�ã���ICombatInterface��ʵ��
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, // ���������
			SpawnTransform,// ������ĳ�ʼ�任��λ�á���ת�����ţ� 
			GetOwningActorFromActorInfo(), // �������������,�����ǽ�ɫActor��SkeletonMeshComponent,�����߱�����AActor ��������������ߺ������߶��ǽ�ɫ�� 
			//��ʹ������һ��AActor���߼������������Ϊ�����˺����㡢�����߼�����ȻӦ�ù����ڽ�ɫ����Ϊ��ɫ�Ǹ��߲�ε��߼�ʵ�塣
			Cast<APawn>(GetOwningActorFromActorInfo()), // ������������ߣ�Instigator��      
			//(��Gameplay Ability System�У�GetOwningActorFromActorInfo()ͨ�����ڻ�ȡ��ǰ���ܵ�ʩ���ߣ�Caster����)

			ESpawnActorCollisionHandlingMethod::AlwaysSpawn // ��ײ����ʽ:��ʾ�����Ƿ�����ײ����������Actor����ʹ����λ�������������赲��������Ҳ�ᱻ���ɡ�
		);

		Projectile->DamageEffectParams = MakeDamgeEffectParamsFormClassDefaults();//�����TargetActor��ʱ��Ϊnullptr��Ϊ����ֻ�������߼��� 
		//TargetActorӦ����EffectActor��BeginOverlap�ٴ����á�

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

