// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
 // 这是一个基于 UAuraDamageGameplayAbility 的自定义技能类，用于实现"光束"类型技能
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	// 存储鼠标点击数据信息
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);
	// 存储拥有者玩家控制器
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

protected:
	// 鼠标命中位置（可被蓝图读写）
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;

	//鼠标命中的Actor（使用TObjectPtr增强指针）
	// 注意：因为有UPROPERTY()，所以这是强引用，会阻止GC回收该Actor
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor>MouseHitActor;

	// 拥有者玩家控制器（使用TObjectPtr增强指针）
	// 因为有UPROPERTY()，所以是强引用，会阻止GC回收PlayerController
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController>OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter>OwnerCharacter;

};
