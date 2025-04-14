// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemGlobals.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraGameplayEffectContext();
}
