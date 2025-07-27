// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraAbilitySystemComponent.generated.h"

struct FGameplayAbilitySpec;



DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTag, const FGameplayTagContainer& /*AssetTag*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)//声明一个多播委托，当技能系统完成初始技能（Startup Abilities）分配时，通知所有订阅者
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)//定义一个委托类型，用于遍历每个技能（FGameplayAbilitySpec），供外部自定义处理逻辑。
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/,int32 /*AbilityLevel*/)
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag&,/* AbilityTag*/ const FGameplayTag&,/* Status*/ const FGameplayTag&, /*Slot */const FGameplayTag& /*PreviousSlot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*AbilityTag*/);

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

	bool bStartupAbilitiesGiven = false;

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFormSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);

	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);


	void UpdateAbilityStatus(int32 Level);
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server,Reliable)
	void SeverUpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void SeverSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void SeverEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);

	UFUNCTION(Client, Reliable)//老师忘写这个了宏了
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilityOfSlot(const FGameplayTag& Slot);
	bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);


protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	virtual void OnRep_ActivateAbilities()override;

	UFUNCTION(Client,Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 AbilityLevel);
};