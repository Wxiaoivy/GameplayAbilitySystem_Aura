// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();// 获取召唤物生成位置的函数，返回位置数组

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	int32 NumMinions = 5;// 要召唤的小兵数量，默认5个

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	TArray<TSubclassOf<APawn>>MinionClasses;// 可召唤的小兵类数组

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	float SpawnSpread = 90;// 小兵生成时的扇形分布角度(度)，默认90度

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Summoning")
	float MinSpawnDistance; // 最小生成距离

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	float MaxSpawnDistance; // 最大生成距离
};
