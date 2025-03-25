// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacter , public IEnemyInterface
{
	GENERATED_BODY()
public:
	/*Enemy Interface*/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/*End Enemy Interface*/


	AAuraEnemy();
protected:
	virtual void BeginPlay()override;

	virtual void InitAbilityActorInfo()override;
	
	UPROPERTY(VisibleAnywhere)
	int32 Level = 1;

	/*Combat Interface*/
	virtual int32 GetPlayerLevel() override;
	/*End Combat Interface*/

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UWidgetComponent* HealthBar;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature  OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature  OnMaxHealthChanged;

};
