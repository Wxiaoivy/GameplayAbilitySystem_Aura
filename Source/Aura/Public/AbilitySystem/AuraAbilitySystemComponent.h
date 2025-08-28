// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraAbilitySystemComponent.generated.h"

struct FGameplayAbilitySpec;
class ULoadScreenSaveGame;



DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTag, const FGameplayTagContainer& /*AssetTag*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)//����һ���ಥί�У�������ϵͳ��ɳ�ʼ���ܣ�Startup Abilities������ʱ��֪ͨ���ж�����
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)//����һ��ί�����ͣ����ڱ���ÿ�����ܣ�FGameplayAbilitySpec�������ⲿ�Զ��崦���߼���
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/,int32 /*AbilityLevel*/)
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag&,/* AbilityTag*/ const FGameplayTag&,/* Status*/ const FGameplayTag&, /*Slot */const FGameplayTag& /*PreviousSlot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*AbilityTag*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag&,/*AibilityTag*/bool/*bActivate*/);//���ڹ㲥 �����ֹͣ�������ܵ�������Ч

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:

	UAuraAbilitySystemComponent();

	//�����ͼ��Ϊʲô������ƣ�
	/*��1����չ��
		���� FGameplayEffectContext ֻ�ṩ�������ܣ�����Դ��Ŀ�꣩��
		������ FAuraGameplayEffectContext ���������Ŀר�����ݣ��� DamageType��DebuffParams��
		��2�����Ͱ�ȫ
		ͨ������ MakeEffectContext()��ȷ��ȫ��ͳһʹ���Զ����ࡣ
		�����ֶ�����ʱ�����ʹ���
		��3���� GAS �ܹ�����
		FGameplayEffectContextHandle ��ͨ�ð�װ����ʵ����������Ŀ������
		���� GAS ���ܣ��� GameplayCues��AttributeSet �ص���Ҳ��ͨ�� GetContext() ������������ࡣ*/
	/*virtual FGameplayEffectContextHandle MakeEffectContext() const override;*///AI˵������д MakeEffectContext() ��ȷ��������ȷ��������ʵ������ʦû��д���Ҿ��ø���д

	void AbilityActorInfoSet();
	
	void AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData);
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
    void AbilityInputTagReleased(const FGameplayTag& InputTag);

	FEffectAssetTag EffectAssetTag;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;
	FAbilityEquipped AbilityEquipped;
	FDeactivatePassiveAbility DeactivatePassiveAbility;
	FActivatePassiveEffect ActivatePassiveEffect;

	bool bStartupAbilitiesGiven = false;

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetSlotTagFromAbilityTag(const FGameplayTag& AbilityTag);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	bool IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec);
	static void AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);


	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);


	void UpdateAbilityStatus(int32 Level);
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server,Reliable)
	void SeverUpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void SeverSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void SeverEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);

	UFUNCTION(Client, Reliable)//UFUNCTION(Client, Reliable),��Ǹú����ڷ��������ã������ڿͻ�����ִ�С���;�����ڷ��������ض��ͻ��˷���֪ͨ�������UI��������Ч�ȣ���
		                       //UFUNCTION(Multicast, Reliable)��Ǹú����ڷ��������ã��ڷ����������пͻ�����ִ�С���;������ͬ��ȫ���¼����籬ըЧ��������״̬�仯�ȣ���
		                       //UFUNCTION(Server, Reliable)��Ǹú����ڿͻ��˵��ã������ڷ�������ִ�С���;�����ڿͻ��������������������������롢�����ȣ�
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	static void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilityOfSlot(const FGameplayTag& Slot);
	bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);


protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	virtual void OnRep_ActivateAbilities()override;

	UFUNCTION(Client,Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 AbilityLevel);
};