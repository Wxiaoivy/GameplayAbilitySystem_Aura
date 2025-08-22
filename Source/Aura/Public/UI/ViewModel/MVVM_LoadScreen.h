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

	// �ɰ󶨵�����
	UPROPERTY(BlueprintReadWrite, FieldNotify)//AI
	FString LoadScreenName;//AI
	// ���÷���
	UFUNCTION(BlueprintCallable)//AI
	void SetLoadScreenName(const FString& NewName);//AI
};