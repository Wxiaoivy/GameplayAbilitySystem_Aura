// Copyright Ivy


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraAbilityTypes.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DamageType, ScaledDamage);

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}


FDamageEffectParams UAuraDamageGameplayAbility::MakeDamgeEffectParamsFormClassDefaults
(AActor* TargetActor /*= nullptr*/, 
	FVector InRadialDamageOrigin /*= FVector::ZeroVector*/,
	bool bOverrideKnockbackDirection /*= false*/, 
	FVector KnockbackDirectionOverride /*= FVector::ZeroVector*/, 
	bool bOverrideDeathImpuse /*= false*/, 
	FVector DeathImpuseOverride /*= FVector::ZeroVector*/, 
	bool bOverridePitch /*= false*/, 
	float PitchOverride /*= 0.f */) const
{

	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DebuffDuration = DebuffDuration;
	Params.DeathImpuseMagnitude = DeathImpuseMagnitude;
	Params.KnockbackChance = KnockbackChance;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;

	// 检查目标Actor是否有效（非空且未被销毁）
	if (IsValid(TargetActor))
	{
		// 计算当前角色到目标Actor的旋转方向（看向目标）
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();

		// 如果启用了俯仰角（Pitch）覆盖，则强制设置指定的俯仰角
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}

		// 获取朝向目标的方向向量（归一化）
		const FVector ToTarget = Rotation.Vector();

		// 如果没有覆盖击退方向，则使用默认朝向目标的击退方向
		if (!bOverrideKnockbackDirection)
		{
			Params.KnockbackForce = ToTarget* KnockbackForceMagnitude;// 计算击退力（方向 × 力度）
		}

		// 如果没有覆盖死亡冲击方向，则使用默认朝向目标的死亡冲击方向
		if (!bOverrideDeathImpuse)
		{
			Params.DeathImpuse = ToTarget * DeathImpuseMagnitude;// 计算死亡冲击力（方向 × 力度）
		}
	}

	// 如果启用了击退方向覆盖
	if (bOverrideKnockbackDirection)
	{
		// 确保击退方向向量是单位向量（归一化）
		KnockbackDirectionOverride.Normalize();

		// 计算击退力（自定义方向 × 力度）
		Params.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude;

		// 如果同时启用了俯仰角覆盖，则调整击退方向的俯仰角
		if (bOverridePitch)
		{
			// 获取自定义方向的旋转
			FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
			// 强制设置俯仰角
			KnockbackRotation.Pitch = PitchOverride;
			// 重新计算击退力（调整后的方向 × 力度）
			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}

	// 如果启用了死亡冲击方向覆盖
	if (bOverrideDeathImpuse)
	{
		// 确保死亡冲击方向向量是单位向量（归一化）
		DeathImpuseOverride.Normalize();

		// 计算死亡冲击力（自定义方向 × 力度）
		Params.DeathImpuse = DeathImpuseOverride * DeathImpuseMagnitude;

		// 如果同时启用了俯仰角覆盖，则调整死亡冲击方向的俯仰角
		if (bOverridePitch)
		{
			// 获取自定义方向的旋转
			FRotator DeathImpuseRotation = DeathImpuseOverride.Rotation();
			// 强制设置俯仰角
			DeathImpuseRotation.Pitch = PitchOverride;
			// 重新计算死亡冲击力（调整后的方向 × 力度）
			Params.DeathImpuse = DeathImpuseRotation.Vector() * DeathImpuseMagnitude;
		}
	}




	// 检查目标Actor是否有效
	if (IsValid(TargetActor))
	{

		if (bIsRadialDamage)
		{
			Params.bIsRadialDamage = bIsRadialDamage;
			Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
			Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
			Params.RadialDamageOrigin = InRadialDamageOrigin;
		}
	}
	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages)const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	return FTaggedMontage();
}
