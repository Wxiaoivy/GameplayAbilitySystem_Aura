// Copyright Ivy


#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;//0-6位即使没有显式声明它们，由于继承或组合关系，这些变量仍然属于 FAuraGameplayEffectContext 的一部分，因此必须在 NetSerialize 中处理，以确保网络同步的正确性。
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid())
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bIsBlockHit)
		{
			RepBits |= 1 << 7;
		}
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 8;
		}
		if (bIsSuccessfulDebuff)
		{
			RepBits |= 1 << 9;
		}
		if (DebuffDamage > 0)
		{
			RepBits |= 1 << 10;
		}
		if (DebuffDuration > 0)
		{
			RepBits |= 1 << 11;
		}
		if (DebuffFrequency > 0)
		{
			RepBits |= 1 << 12;
		}
		if (DamageType->IsValid())
		{
			RepBits |= 1 << 13;
		}
	}
	Ar.SerializeBits(&RepBits, 14);//老师写的13  我觉得是14  因为是Length.

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	//FAuraGameplayEffectContext 中，HitResult 和 DamageType 被设计为 TSharedPtr
	    /*FHitResult 是一个复杂结构，包含位置、法线、碰撞信息等，可能很大。
		FGameplayTag 可能涉及动态标签（如 DamageType 可能是运行时确定的）。
		共享指针允许按需序列化：
		如果 HitResult 或 DamageType 不存在（!IsValid()），则无需序列化，节省带宽。
		在反序列化（Ar.IsLoading()）时，才动态创建对象：
		共享指针确保对象生命周期可控：*/
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	/*WorldOrigin 需要 bHasWorldOrigin，因为：
		FVector（WorldOrigin）没有内置的“有效性”标志，必须额外管理。
		而 bool（如 bIsCriticalHit）本身就是 true / false，不需要额外标志。
		TSharedPtr<FHitResult> 可以用 IsValid() 判断，所以也不需要额外标志。*/
	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockHit;
	}
	if (RepBits & (1 << 8))
	{
		Ar << bIsCriticalHit;
	}
	if (RepBits & (1 << 9))
	{
		Ar << bIsSuccessfulDebuff;
	}
	if (RepBits & (1 << 10))
	{
		Ar << DebuffDamage;
	}
	if (RepBits & (1 << 11))
	{
		Ar << DebuffDuration;
	}
	if (RepBits & (1 << 12))
	{
		Ar << DebuffFrequency;
	}
	if (RepBits & (1 << 13))
	{
		if (Ar.IsLoading())
		{
			if (!DamageType.IsValid())
			{
				DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());
			}
		}
		DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}

	bOutSuccess = true;
	return true;
};
