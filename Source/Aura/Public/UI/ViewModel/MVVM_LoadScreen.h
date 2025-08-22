// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeTitleRequested);

UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UMVVM_LoadScreen();//AI

	// 可绑定的属性
	UPROPERTY(BlueprintReadWrite, FieldNotify)//AI
	FString LoadScreenName;//AI
	// 设置方法
	UFUNCTION(BlueprintCallable)//AI
	void SetLoadScreenName(const FString& NewName);//AI
};