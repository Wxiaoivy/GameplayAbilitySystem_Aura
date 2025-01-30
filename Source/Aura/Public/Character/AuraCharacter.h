// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */


UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController*NewController)override;

	
	virtual void OnRep_PlayerState()override;
	
private:
	void InitAbilityActorInfo();
};
