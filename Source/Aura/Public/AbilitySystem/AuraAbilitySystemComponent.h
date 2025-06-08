// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraAbilitySystemComponent.generated.h"

struct FGameplayAbilitySpec;



DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTag, const FGameplayTagContainer& /*AssetTag*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)//����һ���ಥί�У�������ϵͳ��ɳ�ʼ���ܣ�Startup Abilities������ʱ��֪ͨ���ж�����
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)//����һ��ί�����ͣ����ڱ���ÿ�����ܣ�FGameplayAbilitySpec�������ⲿ�Զ��崦���߼���
DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/)
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:

	UAuraAbilitySystemComponent();

	void AbilityActorInfoSet();
	
	FEffectAssetTag EffectAssetTag;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
    void AbilityInputTagReleased(const FGameplayTag& InputTag);

	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;

	bool bStartupAbilitiesGiven = false;

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);


	void UpdateAbilityStatus(int32 Level);
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server,Reliable)
	void SeverUpgradeAttribute(const FGameplayTag& AttributeTag);


protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	virtual void OnRep_ActivateAbilities()override;

	UFUNCTION(Client,Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag);
};