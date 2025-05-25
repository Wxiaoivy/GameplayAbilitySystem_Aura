// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */


UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController*NewController)override;

	
	virtual void OnRep_PlayerState()override;

	/*Player Interface*/
	 void AddToXP_Implementation(int32 InXP);
	/*End Player Interface*/

	/*Combat Interface*/
	int32 GetPlayerLevel_Implementation();
	/*End Combat Interface*/

private:
	virtual void InitAbilityActorInfo()override;
};
