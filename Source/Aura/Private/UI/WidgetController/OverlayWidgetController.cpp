// Copyright Ivy


#include "UI/WidgetController/OverlayWidgetController.h"

void UOverlayWidgetController::BroadCastInitialValues()
{
	Super::BroadCastInitialValues();//这里可以不要Super  因为父类的实现部分是空白的

	const UAuraAttributeSet* AuraAttributeSet = CastChecked< UAuraAttributeSet>(AttributeSet);

	//广播代理，动态多播代理的绑定在蓝图中实现的（绑定在血条和蓝条的WBP中） 先绑定后广播
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();//这里可以不要Super  因为父类的实现部分是空白的

	const UAuraAttributeSet* AuraAttributeSet = CastChecked< UAuraAttributeSet>(AttributeSet);

	//我们正在获取一个用于监听当前生命值属性变化的委托。并将特定的函数HealthChanged绑定到之前获取的委托上
	//GetGameplayAttributeValueChangeDelegate（）是AbilitySystemComponent自带的一个函数。
	//该代理是一个多播代理（非动态）所以不需要用AddDynamic,就直接用AddUObject。
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	//我们正在获取一个用于监听最大生命值属性变化的委托。并将特定的函数MaxHealthChanged绑定到之前获取的委托上
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTag.AddLambda(
		[](const FGameplayTagContainer& AssetTags)
		{
			// 遍历AssetTags容器中的所有标签。
			for (const FGameplayTag& Tag : AssetTags)
			{
				//TODO:Broadcast the Tag to the WidgetController (TODO: 向WidgetController广播这个标签（这里暂时未实现，仅作提示）)。

				 // 创建一个消息字符串，格式为"GE Tag:标签名称"。
				const FString Msg = FString::Printf(TEXT("GE Tag:%s"), *Tag.ToString());
				// 使用游戏引擎（GEngine）在屏幕上添加一条调试信息。
				// -1表示消息在所有玩家屏幕上显示，8.f是消息显示的时间长度（秒），FColor::Blue是消息的颜色，Msg是消息内容。
				GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);

			}
		}
	);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data)const
{
	OnHealthChanged.Broadcast(Data.NewValue); 
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data)const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}

