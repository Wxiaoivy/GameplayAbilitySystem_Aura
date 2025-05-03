// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraGameplayTags.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
class UBehaviorTreeComponent;
class UCharacterClassInfo;
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacter , public IEnemyInterface
{
	GENERATED_BODY()
public:

	AAuraEnemy();
	/*Enemy Interface*/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
     void SetCombatTarget_Implementation(AActor* InCombatTarget);
	 AActor* GetCombatTarget_Implementation()const ;
	/*End Enemy Interface*/

	/*Combat Interface*/
	virtual int32 GetPlayerLevel() override;
	virtual void die()override;
	
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
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor>CombatTarget;


protected:
	virtual void BeginPlay()override;

	virtual void InitAbilityActorInfo()override;

	virtual void InitializeDefualtAttributes()const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterClassDefaultInfo")
	int32 Level = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterClassDefaultInfo")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UWidgetComponent* HealthBar;

	UPROPERTY(EditAnywhere, Category="AI")
	TObjectPtr<UBehaviorTree>BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController>AuraAIController;

	
};
