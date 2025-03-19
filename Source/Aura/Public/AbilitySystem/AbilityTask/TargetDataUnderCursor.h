// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystemComponent.h"
#include "TargetDataUnderCursor.generated.h"

//用于在目标数据准备好时广播通知，通常用于蓝图中绑定事件。
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/*
 * UTargetDataUnderCursor 类，继承自 UAbilityTask
 * 用于获取鼠标光标下的目标数据，并将数据发送到服务器 
 */
UCLASS()
class AURA_API UTargetDataUnderCursor : public UAbilityTask
{
	GENERATED_BODY()

public:

	//DisplayName = "TargetDataUnderMouse"：在蓝图中显示的函数名称。
	// HidePin = "OwningAbility"：隐藏 OwningAbility 参数（因为 DefaultToSelf 会自动填充该参数）。
	// DefaultToSelf = "OwningAbility"：将 OwningAbility 参数默认设置为调用该函数的 UGameplayAbility 实例。
	// BlueprintInternalUseOnly：表示该函数主要用于蓝图内部逻辑，不建议直接暴露给用户。
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly))
	//static：这是一个静态函数，可以通过类名直接调用，而不需要创建类的实例。
	//UGameplayAbility* OwningAbility：参数表示拥有该 Ability Task 的 UGameplayAbility 实例。
	static UTargetDataUnderCursor* CreatTargetDataUnderMouse(UGameplayAbility* OwningAbility);


	// 声明一个可绑定的多播委托，用于在目标数据准备好时广播
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
	
private:
	virtual void Activate()override;
	void SendMouseCursorData();
};
