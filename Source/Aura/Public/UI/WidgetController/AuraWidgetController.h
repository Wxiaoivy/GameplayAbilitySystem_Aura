// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */

 // 这是一个继承自UObject的类，用于作为UI小部件的控制器，提供玩家控制器、玩家状态、能力系统组件和属性集等信息。
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

protected:

	// 只读属性，存储玩家控制器对象。
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController>PlayerController;


	// 只读属性，存储玩家状态对象。
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState>PlayerState;


	// 只读属性，存储能力系统组件对象。
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent;


	// 只读属性，存储属性集对象。
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet>AttributeSet;
};
