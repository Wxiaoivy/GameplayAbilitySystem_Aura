// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

/**
 * 
 */
class UCharacterClassInfo;
class UAbilityInfo;

UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = "CharacterClassDefaultInfo")
	TObjectPtr<UCharacterClassInfo>CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "AbilityInfo")
	TObjectPtr<UAbilityInfo>AbilityInfo;




};
