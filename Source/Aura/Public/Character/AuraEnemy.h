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

#include "AuraEnemy.generated.h"

/**
 * 
 */
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
	/*End Enemy Interface*/

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature  OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature  OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	virtual void die()override;


protected:
	virtual void BeginPlay()override;

	virtual void InitAbilityActorInfo()override;

	virtual void InitializeDefualtAttributes()const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterClassDefaultInfo")
	int32 Level = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterClassDefaultInfo")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	/*Combat Interface*/
	virtual int32 GetPlayerLevel() override;
	/*End Combat Interface*/

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UWidgetComponent* HealthBar;


};
