// Copyright Ivy


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadCastInitialValues()
{
	
	check(AttributeInfo);
	//遍历 AS->TagsToAttributes 中的键值对,并储存在Pair中。
	for (auto& Pair : GetAuraAS()->TagsToAttributes)
	{
		//FAuraAttributeInfo是AttributeInfo类中的结构体
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
		//这里的Pair.Value是个静态函数指针，通过函数指针调用函数时，需要在函数指针后面加 ()，并传递参数：这里没有参数不用传参。
		//Pair.Value:这是一个静态函数指针，类型是 TStaticFuncPtr<FGameplayAttribute()>，即 FGameplayAttribute(*)()。
        // 它指向一个返回 FGameplayAttribute 且没有参数的静态函数。
		// Pair.Value() :通过函数指针调用函数，返回一个 FGameplayAttribute 对象。
		// GetNumericValue(AS) :这是 FGameplayAttribute 类的成员函数，用于获取属性的数值。
		Info.AttributeValue = Pair.Value().GetNumericValue(AuraAttributeSet);
		//代理广播传递Info
		AttributeInfoDelegate.Broadcast(Info);
	}
	

	
	OnAttributePointsChangedDelegateInController.Broadcast(AuraPlayerState->GetAttributePoints());
	//OnAttributePointsChangedDelegateInController.Clear();//这行是我自己查AI加的 因为每次按菜单按钮初始化时  第二次会执行2次代理  第三次会执行3次代理，我这儿防止重复绑定。(不能加这行不然会出问题)
}

	void UAttributeMenuWidgetController::BindCallbacksToDependencies()
	{
		//UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
		check(AttributeInfo);
		for (auto& Pair : GetAuraAS()->TagsToAttributes)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
				[this,Pair](const FOnAttributeChangeData& Data)//虽然Data函数体里没调用 但是这个GetGameplayAttributeValueChangeDelegate要求被绑定的函数必须有这个参数
				{
					FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
					Info.AttributeValue = Pair.Value().GetNumericValue(GetAuraAS());

					AttributeInfoDelegate.Broadcast(Info);
				}
			);
		}
		//AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
		GetAuraPS()->OnAttirbutePointsChangedDelegate.AddLambda
		([this](int32 InPoints)
			{
				OnAttributePointsChangedDelegateInController.Broadcast(InPoints);
			}
		);
	}

	void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
	{
	     GetAuraASC()->UpgradeAttribute(AttributeTag);
	}
