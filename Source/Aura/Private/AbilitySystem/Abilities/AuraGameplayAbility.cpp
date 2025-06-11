// Copyright Ivy


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString();
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString();
}

FString UAuraGameplayAbility::GetLockedDescription(int32 RequirementLevel)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level %d</>"), RequirementLevel);
}
