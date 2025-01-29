// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */

 // 这是一个继承自UUserWidget的类，用于自定义用户界面小部件。
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// 一个可以在蓝图（Unreal Engine的可视化脚本系统）中调用的函数，用于设置WidgetController。
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	// 一个在蓝图中只读的属性，用于存储WidgetController对象。
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject>WidgetController;

protected:
	// 一个可以在蓝图中被重写的事件函数，当SetWidgetController()被设置时调用。
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

	
};
