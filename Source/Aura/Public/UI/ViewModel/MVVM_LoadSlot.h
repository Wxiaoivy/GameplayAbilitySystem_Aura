// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton,bool,bEnable);
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:


	/*Setter*/
	//UFUNCTION(BlueprintCallable)
	void SetLoadSlotName(const FString& InLoadSlotName);

	//UFUNCTION(BlueprintCallable)
	void SetPlayerName(FString InPlayerName);

	//UFUNCTION(BlueprintCallable)
	void SetMapName(FString InMapName);



	/*Getter*/
	//UFUNCTION(BlueprintCallable)
	const FString& GetPlayerName() const {return PlayerName;};

	//UFUNCTION(BlueprintCallable)
	const FString& GetMapName() const { return MapName; };



	/*Delegate*/
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;


	void InitializeSlot();

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus>SlotStatus;

	UPROPERTY()
	int32 SlotIndex;

	/*Field Notify*/
	UPROPERTY(BlueprintReadWrite, FieldNotify)
	FString LoadSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter = SetPlayerName, Getter = GetPlayerName/*, meta = (AllowPrivateAccess = "true")*/);
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter = SetMapName, Getter = GetMapName);
	FString MapName;
};
