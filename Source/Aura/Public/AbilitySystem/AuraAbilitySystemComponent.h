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
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)//声明一个多播委托，当技能系统完成初始技能（Startup Abilities）分配时，通知所有订阅者
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)//定义一个委托类型，用于遍历每个技能（FGameplayAbilitySpec），供外部自定义处理逻辑。
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/,int32 /*AbilityLevel*/)
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag&,/* AbilityTag*/ const FGameplayTag&,/* Status*/ const FGameplayTag&, /*Slot */const FGameplayTag& /*PreviousSlot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*AbilityTag*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag&,/*AibilityTag*/bool/*bActivate*/);//用于广播 激活和停止被动技能的粒子特效

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:

	UAuraAbilitySystemComponent();

	//设计意图：为什么这样设计？
	/*（1）扩展性
		基类 FGameplayEffectContext 只提供基础功能（如来源、目标）。
		派生类 FAuraGameplayEffectContext 可以添加项目专属数据（如 DamageType、DebuffParams）
		（2）类型安全
		通过重载 MakeEffectContext()，确保全局统一使用自定义类。
		避免手动创建时的类型错误。
		（3）与 GAS 架构兼容
		FGameplayEffectContextHandle 是通用包装器，实际类型由项目决定。
		其他 GAS 功能（如 GameplayCues、AttributeSet 回调）也能通过 GetContext() 访问你的派生类。*/
	/*virtual FGameplayEffectContextHandle MakeEffectContext() const override;*///AI说必须重写 MakeEffectContext() 以确保返回正确的派生类实例。老师没重写，我觉得该重写

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

	UFUNCTION(Client, Reliable)//UFUNCTION(Client, Reliable),标记该函数在服务器调用，但仅在客户端上执行。用途：用于服务器向特定客户端发送通知（如更新UI、播放音效等）。
		                       //UFUNCTION(Multicast, Reliable)标记该函数在服务器调用，在服务器和所有客户端上执行。用途：用于同步全局事件（如爆炸效果、场景状态变化等）。
		                       //UFUNCTION(Server, Reliable)标记该函数在客户端调用，但仅在服务器上执行。用途：用于客户端向服务器发送请求（如玩家输入、交互等）
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