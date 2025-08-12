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

	// ���Ŀ��Actor�Ƿ���Ч���ǿ���δ�����٣�
	if (IsValid(TargetActor))
	{
		// ���㵱ǰ��ɫ��Ŀ��Actor����ת���򣨿���Ŀ�꣩
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();

		// ��������˸����ǣ�Pitch�����ǣ���ǿ������ָ���ĸ�����
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}

		// ��ȡ����Ŀ��ķ�����������һ����
		const FVector ToTarget = Rotation.Vector();

		// ���û�и��ǻ��˷�����ʹ��Ĭ�ϳ���Ŀ��Ļ��˷���
		if (!bOverrideKnockbackDirection)
		{
			Params.KnockbackForce = ToTarget* KnockbackForceMagnitude;// ��������������� �� ���ȣ�
		}

		// ���û�и����������������ʹ��Ĭ�ϳ���Ŀ��������������
		if (!bOverrideDeathImpuse)
		{
			Params.DeathImpuse = ToTarget * DeathImpuseMagnitude;// ������������������� �� ���ȣ�
		}
	}

	// ��������˻��˷��򸲸�
	if (bOverrideKnockbackDirection)
	{
		// ȷ�����˷��������ǵ�λ��������һ����
		KnockbackDirectionOverride.Normalize();

		// ������������Զ��巽�� �� ���ȣ�
		Params.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude;

		// ���ͬʱ�����˸����Ǹ��ǣ���������˷���ĸ�����
		if (bOverridePitch)
		{
			// ��ȡ�Զ��巽�����ת
			FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
			// ǿ�����ø�����
			KnockbackRotation.Pitch = PitchOverride;
			// ���¼����������������ķ��� �� ���ȣ�
			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}

	// �������������������򸲸�
	if (bOverrideDeathImpuse)
	{
		// ȷ������������������ǵ�λ��������һ����
		DeathImpuseOverride.Normalize();

		// ����������������Զ��巽�� �� ���ȣ�
		Params.DeathImpuse = DeathImpuseOverride * DeathImpuseMagnitude;

		// ���ͬʱ�����˸����Ǹ��ǣ�����������������ĸ�����
		if (bOverridePitch)
		{
			// ��ȡ�Զ��巽�����ת
			FRotator DeathImpuseRotation = DeathImpuseOverride.Rotation();
			// ǿ�����ø�����
			DeathImpuseRotation.Pitch = PitchOverride;
			// ���¼��������������������ķ��� �� ���ȣ�
			Params.DeathImpuse = DeathImpuseRotation.Vector() * DeathImpuseMagnitude;
		}
	}




	// ���Ŀ��Actor�Ƿ���Ч
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
