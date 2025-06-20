// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include <../../../../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h>
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemComponent.h>
#include "CombatInterface.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnASCRegistered, UAbilitySystemComponent*)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);



class UAnimMontage;
USTRUCT(BlueprintType)
struct FTaggedMontage
{
   GENERATED_BODY()
    
   UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
   UAnimMontage* Montage = nullptr;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
   FGameplayTag MontageTag;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
   FGameplayTag CombatSocketTag;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
   USoundBase* ImpactSound;

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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	 int32 GetPlayerLevel();

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IncrementalMinionCount(int32 Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass();


	virtual void die() = 0; //��Ҫ�߼�д��AuraCharacterBase�����
	virtual FOnASCRegistered GetOnASCRegisteredDelegate() = 0;
	virtual FOnDeath GetOnDeathDelegate() = 0;
};
