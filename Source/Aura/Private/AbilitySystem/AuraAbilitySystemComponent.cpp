// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemComponent.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent()
{

}

//这个函数被AuraCharacterBase的同名函数调用
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	// 这是一个 for 循环，用于遍历 StartupAbilities 数组中的每一个能力类。
	// TSubclassOf<UGameplayAbility>AbilityClass: 每次循环时，AbilityClass 会代表数组中的一个能力类。
	// StartupAbilities: 是传入的能力数组。
	for (const TSubclassOf<UGameplayAbility>AbilityClass : StartupAbilities)
	{
		//FGameplayAbilitySpec: 是 Gameplay Ability System (GAS) 中用来描述能力实例的结构体。
		// AbilityClass: 当前循环中的能力类。
		// 1 : 表示能力的等级（Level），这里设置为 1。
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		//检查 AbilitySpec.Ability 是否是 UAuraGameplayAbility 类型的实例。
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast< UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//如果是，则将一个输入标签（StartupInputTag）添加到能力实例的 DynamicAbilityTags 中
			//AbilitySpec.DynamicAbilityTags: 是能力实例的动态标签集合，用于存储与能力相关的标签。
			//AuraGameplayAbility->StartupInputTag: 是 UAuraGameplayAbility 中定义的输入标签，表示该能力对应的输入（如按键）。
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupInputTag);
			//将能力实例（AbilitySpec）添加到角色的 AbilitySystemComponent 中。
			GiveAbility(AbilitySpec);
		}
		                    

	   //将能力实例（AbilitySpec）添加到角色的 AbilitySystemComponent 中。
	   //解释:
	   //GiveAbility: 是 AbilitySystemComponent 的一个函数，用于将能力添加到角色中。
       //AbilitySpec : 是上一步创建的能力实例。
	   //GiveAbility(AbilitySpec);//用这个函数可以在上一行代码写const（区别）
		
	
	   //GiveAbilityAndActivateOnce: 是 AbilitySystemComponent 的一个函数，用于添加能力并立即激活。
		//GiveAbilityAndActivateOnce(AbilitySpec);//用这个函数不可以在上面第二行代码写const（区别）


		//GiveAbilityAndActivateOnce 会立即激活能力，如果不需要立即激活，可以只使用 GiveAbility。

		//如果能力需要特定的激活条件（如按键触发），则不需要调用 GiveAbilityAndActivateOnce。
	}
}

//这个函数被AuraPlayerController的同名函数调用
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);

			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

//这个函数被AuraPlayerController的同名函数调用
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);

		}
	}

}

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//OnGameplayEffectAppliedDelegateToSelf是父类的一个代理实例，这里绑定回调函数到此代理
	//OnGameplayEffectAppliedDelegateToSelf每当一个游戏效果（Game Effect，简称GE）被应用到自身上时，无论是即时效果还是持续效果，服务器都会调用此方法。”
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

	//通过FAuraGameplayTags::Get()方法获得了该类的唯一单例储存在GameplayTags中
	//通过实例可以访问Attribute_Secondary_Armor成员变量
	//const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Orange, FString::Printf(TEXT("Tag:%s"), *GameplayTags.Attribute_Secondary_Armor.ToString()));
	// FGameplayTag Tag = FAuraGameplayTags::Get().Attribute_Primary_Intelligence;//测试可以在C++中访问这些native tags
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{

	// 定义一个游戏玩法标签容器（TagContainer），用于存储从效果规范中提取的标签。
	FGameplayTagContainer TagContainer;
	// 从效果规范中获取所有的资产标签，并将它们存储在TagContainer中。
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTag.Broadcast(TagContainer);//广播代理到WidgetController（发送数据）
}

