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
	TArray<FVector> GetSpawnLocations();// ��ȡ�ٻ�������λ�õĺ���������λ������

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	int32 NumMinions = 5;// Ҫ�ٻ���С��������Ĭ��5��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	TArray<TSubclassOf<APawn>>MinionClasses;// ���ٻ���С��������

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	float SpawnSpread = 90;// С������ʱ�����ηֲ��Ƕ�(��)��Ĭ��90��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Summoning")
	float MinSpawnDistance; // ��С���ɾ���

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	float MaxSpawnDistance; // ������ɾ���
};
