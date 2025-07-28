// Copyright Ivy


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemBlueprintLibrary.h>
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraAbilitySystemComponent*AuraASC=Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
	}
	else if (ICombatInterface* CombatInterface=Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda
		(
			[this](UAbilitySystemComponent* ASC)//����ģ�UAbilitySystemComponent* ASC�� �����������ӣ���Ϊ��GetOnASCRegisteredDelegate�������Ҫ���ݵĲ���
			{
				if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
				{
					AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				}
			}
		);
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate&&!IsActive())
		{
			Activate();
		} 
		else
		{
			Deactivate();
		}
		
	}
}
