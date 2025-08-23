// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:

	/*AI�ӵ�  Ϊ����ȷʹ��MVVM*/
	UFUNCTION(BlueprintCallable)
	void SetLoadSlotName(const FString& InLoadSlotName);

	UPROPERTY(BlueprintReadWrite, FieldNotify)
	FString LoadSlotName;
	/*AI�ӵ�  Ϊ����ȷʹ��MVVM*/

	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	void InitializeSlot();
};
