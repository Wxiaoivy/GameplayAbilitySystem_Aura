// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
//#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
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


};
