// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "WaitCooldownChange.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);//定义一个可以动态绑定的多播委托（允许多个地方监听） float TimeRemaining：回调时传递的参数（剩余冷却时间）。

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase//UBlueprintAsyncActionBase：UE提供的异步任务基类，用来做后台任务。
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)//UPROPERTY(BlueprintAssignable)：让这个委托可以在蓝图中绑定。
	FCooldownChangeSignature CooldownStart;// 冷却开始时触发

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd; // 冷却结束时触发

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))//为什么用静态方法？因为异步任务需要先创建实例，再开始监听，静态方法可以直接调用，方便蓝图使用。
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbillitySystemComponent, const FGameplayTag& InCooldownTag);
	UFUNCTION(BlueprintCallable)
	void EndTask();//作用：停止监听冷却事件（比如玩家取消了技能）。为什么需要这个？如果不手动终止，即使技能结束了，委托仍然会监听，可能导致内存泄漏。

protected:

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>ASC;

	FGameplayTag CooldownTag;

	void CooldownTagChange(const FGameplayTag InCooldownTag, int32 NewCount);//当冷却标签数量变化时调用（比如冷却结束）。
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle); //当技能效果（GameplayEffect）被添加到角色时调用。

	/*为什么需要这两个回调函数？
		因为冷却可能由技能效果触发，也可能由标签直接控制，所以要监听两种事件。*/


};
