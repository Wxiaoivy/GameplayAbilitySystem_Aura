// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "Interaction/PlayerInterface.h"
#include <../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "Interaction/SaveInterface.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

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
	 int32 GetXP_Implementation()const;
	 int32 FindLevelForXP_Implementation(int32 InXP)const;
	 void LevelUp_Implementation();
	 void AddToSpellPoints_Implementation(int32 InSpellPoints);
	 void AddToPlayerLevel_Implementation(int32 InPlayerLevel);
	 void AddToAttributePoints_Implementation(int32 InAttributePoints);
	 int32 GetAttributePointsReward_Implementation(int32 Level)const;
	 int32 GetSpellPointsReward_Implementation(int32 Level)const;
	 int32 GetAttributePoints_Implementation()const;
	 int32 GetSpellPoints_Implementation()const;
	 void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial);
	 void HideMagicCircle_Implementation();
	 void SaveProgress_Implementation(const FName& CheckPointTag);
	/*End Player Interface*/

	/*Combat Interface*/
	int32 GetPlayerLevel_Implementation();
	virtual void  die(const FVector& DeathImpuse)override;
	/*End Combat Interface*/

	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	virtual void OnRep_Stunned()override;

	void LoadProgress();

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	virtual void InitAbilityActorInfo()override;
    
	UFUNCTION(NetMulticast,Reliable)
	void MulticastLevelupParticles()const;
};
