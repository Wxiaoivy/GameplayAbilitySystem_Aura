// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadScreenWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// 将 BlueprintImplementableEvent 改为 BlueprintNativeEvent
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BluePrintInitializeWidget();

	// 注意：这里需要声明对应的_Implementation函数
	virtual void BluePrintInitializeWidget_Implementation();
};
