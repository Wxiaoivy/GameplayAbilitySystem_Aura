// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "OverlayWidgetController.generated.h"


class UAuraUserWidget;
class UAbilityInfo;
class UAuraAbilitySystemComponent;
USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;

};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);//声明一个蓝图可绑定的动态委托，用于将技能信息（FAuraAbilityInfo）广播到UI（如技能图标、冷却时间）。

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
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Message")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Message")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnStatChangedSignature OnPlayerLevelChangedDelegate;

protected:

	//FOnAttributeChangeData 是一个自定义的结构体或类，专门设计用于传递属性变化的信息。
	//它包含多个成员变量，比如旧值（OldValue）、新值（NewValue）、属性名称（AttributeName）等，用于详细描述属性变化的各个方面。

	//在您的 HealthChanged 函数中，FOnAttributeChangeData 是作为参数传入的。这意味着在某个地方（可能是属性变化的触发点），
	//有一个代码片段创建了一个 FOnAttributeChangeData 实例，并填充了相关的变化信息，然后调用了 HealthChanged 函数，将这个实例作为参数传递。
	//void HealthChanged(const FOnAttributeChangeData& Data)const;

	//void MaxHealthChanged(const FOnAttributeChangeData& Data)const;

	//void ManaChanged(const FOnAttributeChangeData& Data)const;

	//void MaxManaChanged(const FOnAttributeChangeData& Data)const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable>MessageWidgetDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo>AbilityInfo;


	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC);


	//这里的T表示无论什么类型的DataTable;是个模板函数，可以从任何类型的DataTable获取信息。
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	void OnXPChange(int32 NewXP)const;

}; 

template<typename T>
inline T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	//通常是通过RowName来获取该行（Row）的信息，但是我们在DataTable里把RowName直接写成了TagName所以可以直接利用Tag.GetTagName()来获得RowName.

	T* Row = DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));//TEXT("")没什么意义 只是为了凑参数的。
	return Row;
}
