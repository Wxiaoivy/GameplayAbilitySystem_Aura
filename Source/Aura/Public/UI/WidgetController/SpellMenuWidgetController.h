// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "HAL/Platform.h" 
#include <../../../../../../../Source/Runtime/GameplayTags/Classes/GameplayTagContainer.h>
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"


// 声明一个动态多播委托，可以广播给多个接收者，能在蓝图中使用
// 这个委托用来通知法术球体被选中时两个按钮的状态
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled, FString, CurrentLevelDescription, FString, NextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

/**
 * 
 */

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};


UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	virtual void BroadCastInitialValues()override;// 重写父类的广播初始值方法	

	virtual void BindCallbacksToDependencies()override; // 重写绑定回调方法


	UPROPERTY(BlueprintAssignable)
	FOnStatChangedSignature OnSpellPointsChangedDelegateOnController;// 法术点数变化委托

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;// 法术球选中委托

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;


	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag); // 法术球被选中时调用的函数，可以在蓝图中调用

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GlobeDeselected();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag,const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

 private:
	 //静态辅助函数，FORCENOINLINE告诉编译器不要内联这个函数
	 // - AbilityStatus: 能力当前状态标签
	 // - SpellPoints: 玩家当前拥有的法术点数
	 // - bShouldEnableSpendPointsButton: 输出参数，是否启用花费点数按钮
	 // - bShouldEnableEquipButton: 输出参数，是否启用装备按钮
	 static void FORCENOINLINE  ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton);

	 FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None,FAuraGameplayTags::Get().Abilities_Status_Lock };//初始化这个结构体，每当选择法术球时都要重新设置该结构体
	 int32 CurrentSpellPoints = 0;

	 bool bWaitingForEquipSelection = false;

	 FGameplayTag SelectedSlot;
};
