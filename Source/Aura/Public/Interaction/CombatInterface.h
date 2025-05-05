// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"



class UAnimMontage;
USTRUCT(BlueprintType)
struct FTaggedMontage
{
   GENERATED_BODY()
    
   UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
   UAnimMontage* Montage = nullptr;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
   FGameplayTag MontageTag;

};


// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//当你在接口中声明一个 BlueprintNativeEvent 函数时，
	/*Unreal Header Tool(UHT) 会自动生成以下代码：

		一个纯虚函数 GetCombatSocketLocation()（必须由 C++ 子类实现）。
		一个默认实现 GetCombatSocketLocation_Implementation()（可选覆盖）。
		一个全局执行函数 Execute_GetCombatSocketLocation(UObject * Object)（用于动态调用）。*/

	virtual int32 GetPlayerLevel();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor*GetAvatar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    TArray<FTaggedMontage>GetAttackMontages();

	virtual void die() = 0; //主要逻辑写在AuraCharacterBase里面的
};
