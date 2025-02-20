// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemComponent.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent()
{

}

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//OnGameplayEffectAppliedDelegateToSelf是父类的一个代理实例，这里绑定回调函数到此代理
	//OnGameplayEffectAppliedDelegateToSelf每当一个游戏效果（Game Effect，简称GE）被应用到自身上时，无论是即时效果还是持续效果，服务器都会调用此方法。”
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{

	// 定义一个游戏玩法标签容器（TagContainer），用于存储从效果规范中提取的标签。
	FGameplayTagContainer TagContainer;
	// 从效果规范中获取所有的资产标签，并将它们存储在TagContainer中。
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTag.Broadcast(TagContainer);//广播代理到WidgetController（发送数据）
}

