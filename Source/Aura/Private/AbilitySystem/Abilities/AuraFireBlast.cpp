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

// 定义一个返回TArray<AAuraFireBall*>的函数，用于生成多个火球
TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	// 创建一个空的火球数组
	TArray<AAuraFireBall*>FireBalls;
	// 从ActorInfo获取角色前向向量
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	// 从ActorInfo获取角色位置
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	// 使用自定义的工具函数均匀分布旋转角度，生成NumFireBalls各个旋转角度
	TArray<FRotator>Rotators = UAuraAbilitySystemLibrary::EvenlySpaceRotators(Forward, FVector::UpVector, 360, NumFireBalls);

	// 遍历每个旋转角度
	for (const FRotator& Rotator : Rotators)
	{
		// 创建生成变换	
		FTransform SpawnTransform;
		// 设置旋转（四元数形式）
		SpawnTransform.SetRotation(Rotator.Quaternion());
		// 设置位置
		SpawnTransform.SetLocation(Location);

		// 延迟生成火球actor
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>
			(
				FireBallClass,// 要生成的actor类
				SpawnTransform,// 生成变换
				GetOwningActorFromActorInfo(),// 拥有者actor
				CurrentActorInfo->PlayerController->GetPawn(),// Instigator(触发者 / 发起者)：伤害的责任归属（谁“造成”了这个技能）
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
		/*Instigator 应该用法杖还是角色？
		  永远用角色！因为：
		  法杖是“工具”，角色是“使用者”。
		  伤害统计、击杀记录等逻辑需要关联到角色，而非武器。*/

		  /*5. 总结：法杖技能的最佳实践
			  AvatarActor:
			  如果需要法杖独立逻辑 → 用法杖自身。
			  如果只是视觉部件 → 用角色，通过Socket获取法杖位置。

			  OwnerActor
			  永远是持有法杖的角色（玩家 / AI），负责技能资源管理。(比如耗蓝）

			  Instigator
			  永远是角色（玩家 / AI），用于伤害归属。*/


		FireBall->DamageEffectParams = MakeDamgeEffectParamsFormClassDefaults();

		FireBalls.Add(FireBall);

		FireBall->FinishSpawning(SpawnTransform);
	}









	return TArray<AAuraFireBall*>();
}
