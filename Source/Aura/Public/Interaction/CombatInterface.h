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
	//�����ڽӿ�������һ�� BlueprintNativeEvent ����ʱ��
	/*Unreal Header Tool(UHT) ���Զ��������´��룺

		һ�����麯�� GetCombatSocketLocation()�������� C++ ����ʵ�֣���
		һ��Ĭ��ʵ�� GetCombatSocketLocation_Implementation()����ѡ���ǣ���
		һ��ȫ��ִ�к��� Execute_GetCombatSocketLocation(UObject * Object)�����ڶ�̬���ã���*/

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

	virtual void die() = 0; //��Ҫ�߼�д��AuraCharacterBase�����
};
