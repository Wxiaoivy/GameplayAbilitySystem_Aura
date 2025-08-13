// Copyright Ivy


#include "AbilitySystem/Abilities/AuraFireBlast.h"
#include "Actor/AuraFireBall.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	
		return FString::Printf(TEXT
		(

			//Title
			"<Title>FIRE BLAST</>\n\n"

			//Level
			"<Small>Level: </><Level>%d</>\n"
			//ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n"

			//Number of FireBalls
			"<Default>Launches %d </>"
			"<Default>fire balls in all directions, each coming back and </>"
			"Exploding upon return, causing </>"

			//Damage
			"<Damage>%d</><Default>radial fire damage with a chance to burn . </>"

		),
			//Values
			Level,
			ManaCost,
			Cooldown,
			NumFireBalls,
			ScaledDamage
		);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
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

		//Number of FireBalls
		"<Default>Launches %d </>"
		"<Default>fire balls in all directions, each coming back and </>"
		"Exploding upon return, causing </>"

		//Damage
		"<Damage>%d</><Default>radial fire damage with a chance to burn . </>"

	),
		//Values
		Level,
		ManaCost,
		Cooldown,
		NumFireBalls,
		ScaledDamage
	);
}

// ����һ������TArray<AAuraFireBall*>�ĺ������������ɶ������
TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	// ����һ���յĻ�������
	TArray<AAuraFireBall*>FireBalls;
	// ��ActorInfo��ȡ��ɫǰ������
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	// ��ActorInfo��ȡ��ɫλ��
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	// ʹ���Զ���Ĺ��ߺ������ȷֲ���ת�Ƕȣ�����NumFireBalls������ת�Ƕ�
	TArray<FRotator>Rotators = UAuraAbilitySystemLibrary::EvenlySpaceRotators(Forward, FVector::UpVector, 360, NumFireBalls);

	// ����ÿ����ת�Ƕ�
	for (const FRotator& Rotator : Rotators)
	{
		// �������ɱ任	
		FTransform SpawnTransform;
		// ������ת����Ԫ����ʽ��
		SpawnTransform.SetRotation(Rotator.Quaternion());
		// ����λ��
		SpawnTransform.SetLocation(Location);

		// �ӳ����ɻ���actor
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>
			(
				FireBallClass,// Ҫ���ɵ�actor��
				SpawnTransform,// ���ɱ任
				GetOwningActorFromActorInfo(),// ӵ����actor
				CurrentActorInfo->PlayerController->GetPawn(),// Instigator(������ / ������)���˺������ι�����˭����ɡ���������ܣ�
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
		/*Instigator Ӧ���÷��Ȼ��ǽ�ɫ��
		  ��Զ�ý�ɫ����Ϊ��
		  �����ǡ����ߡ�����ɫ�ǡ�ʹ���ߡ���
		  �˺�ͳ�ơ���ɱ��¼���߼���Ҫ��������ɫ������������*/

		  /*5. �ܽ᣺���ȼ��ܵ����ʵ��
			  AvatarActor:
			  �����Ҫ���ȶ����߼� �� �÷�������
			  ���ֻ���Ӿ����� �� �ý�ɫ��ͨ��Socket��ȡ����λ�á�

			  OwnerActor
			  ��Զ�ǳ��з��ȵĽ�ɫ����� / AI������������Դ����(���������

			  Instigator
			  ��Զ�ǽ�ɫ����� / AI���������˺�������*/


		FireBall->DamageEffectParams = MakeDamgeEffectParamsFormClassDefaults();

		FireBalls.Add(FireBall);

		FireBall->FinishSpawning(SpawnTransform);
	}









	return TArray<AAuraFireBall*>();
}
