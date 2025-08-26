// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

class UMVVM_LoadSlot;

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeTitleRequested);

UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:

	/*AI加的  为了正确使用MVVM*/

	UMVVM_LoadScreen();//AI

	// 可绑定的属性
	UPROPERTY(BlueprintReadWrite, FieldNotify)//AI
	FString LoadScreenName;//AI
	// 设置方法
	UFUNCTION(BlueprintCallable)//AI
	void SetLoadScreenName(const FString& NewName);//AI

	/*AI加的  为了正确使用MVVM*/

	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;

	void InitializeLoadSlots();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot>LoadSlotViewModelClass;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index)const;

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot,const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelecteSlotButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();

	void LoadData();

private:

	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*>LoadSlots;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot>LoadSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot>LoadSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot>LoadSlot_2;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot>SelectedSlot;

};