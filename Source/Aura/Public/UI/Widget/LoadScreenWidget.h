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

	// �� BlueprintImplementableEvent ��Ϊ BlueprintNativeEvent
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BluePrintInitializeWidget();

	// ע�⣺������Ҫ������Ӧ��_Implementation����
	virtual void BluePrintInitializeWidget_Implementation();
};
