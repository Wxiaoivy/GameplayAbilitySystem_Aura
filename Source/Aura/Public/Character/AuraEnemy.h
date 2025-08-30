// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraGameplayTags.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Interaction/HighlightInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
class UBehaviorTreeComponent;
class UCharacterClassInfo;
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase , public IEnemyInterface , public IHighlightInterface
{
	GENERATED_BODY()
public:

	AAuraEnemy();


	/*Hightlight Interface*/
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination)override;
	/*End Hightlight Interface*/
	

	/*Combat Interface*/
	int32 GetPlayerLevel_Implementation();
	virtual void die(const FVector& DeathImpuse)override;
	void SetCombatTarget_Implementation(AActor* InCombatTarget);
	AActor* GetCombatTarget_Implementation()const;
	/*End Combat Interface*/

	virtual void PossessedBy(AController* NewController)override;//PossessedBy()是角色被控制器接管时自动触发；（角色出生，重生，控制器变更都会触发）

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature  OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature  OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bHitReacting = false;



	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor>CombatTarget;


protected:
	virtual void BeginPlay()override;

	virtual void InitAbilityActorInfo()override;

	virtual void InitializeDefualtAttributes()const override;

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterClassDefaultInfo")
	int32 Level = 1;

	

	

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UWidgetComponent* HealthBar;

	UPROPERTY(EditAnywhere, Category="AI")
	TObjectPtr<UBehaviorTree>BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController>AuraAIController;

	
};
