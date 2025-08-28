// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include <../../../../../../../Source/Runtime/GameplayTags/Classes/GameplayTagContainer.h>
#include "PassiveNiagaraComponent.generated.h"

class UAuraAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UPassiveNiagaraComponent();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;
	
protected:
	virtual void BeginPlay()override;
	void ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC);
	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);
};
