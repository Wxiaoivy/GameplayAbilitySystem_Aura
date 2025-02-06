// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature, float, NewMaxMana);

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadCastInitialValues()override;

	virtual void BindCallbacksToDependencies()override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnManaChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnMaxManaChangedSignature OnMaxManaChanged;


protected:

	//FOnAttributeChangeData 是一个自定义的结构体或类，专门设计用于传递属性变化的信息。
	//它包含多个成员变量，比如旧值（OldValue）、新值（NewValue）、属性名称（AttributeName）等，用于详细描述属性变化的各个方面。

	//在您的 HealthChanged 函数中，FOnAttributeChangeData 是作为参数传入的。这意味着在某个地方（可能是属性变化的触发点），
	//有一个代码片段创建了一个 FOnAttributeChangeData 实例，并填充了相关的变化信息，然后调用了 HealthChanged 函数，将这个实例作为参数传递。
	void HealthChanged(const FOnAttributeChangeData& Data)const;

	void MaxHealthChanged(const FOnAttributeChangeData& Data)const;

	void ManaChanged(const FOnAttributeChangeData& Data)const;

	void MaxManaChanged(const FOnAttributeChangeData& Data)const;
}; 
