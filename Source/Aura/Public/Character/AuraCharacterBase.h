// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"
#include "AuraCharacterBase.generated.h"



UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	TObjectPtr<UAttributeSet>GetAttributeSet() const { return AttributeSet; }

	

	

	//NetMulticast：表示这是一个 多播 RPC，会在服务器调用后同步到所有客户端。
	//Reliable：保证该 RPC 必定会执行（即使网络条件差，也会重试直到成功）。
	/*关键点：

		必须在服务器调用。

		使用 _Implementation 实现函数。

		用 Reliable 确保关键逻辑不丢失。*/
	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//跟AbilitySystem里面一个函数一样的名字（这个有点坑）
	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attribute")
	TSubclassOf<UGameplayEffect>DefaultPrimaryAttribute;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attribute")
	TSubclassOf<UGameplayEffect>DefaultSecondaryAttribute;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attribute")
	TSubclassOf<UGameplayEffect>DefaultVitalAttribute;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;
	

	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffect, float level)const;
	virtual void InitializeDefualtAttributes()const;

	void AddCharacterAbilities();

	/*Dissolve Effect*/
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMatInst);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMatInst);

	/*Dissolve Effect*/

	/*Minions*/

	int32 MinionCount = 0;

	/*Minions*/



 public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent>Weapon;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr <UAttributeSet>AttributeSet;

	/*CombatInterface*/

	//Unreal Header Tool(UHT) 会自动生成以下代码：
	//	一个纯虚函数 GetCombatSocketLocation()（必须由 C++ 子类实现）。
	//	一个默认实现 GetCombatSocketLocation_Implementation()（可选覆盖）。
	//	一个全局执行函数 Execute_GetCombatSocketLocation(UObject * Object)（用于动态调用）。
	FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag);//得到武器上面发射抛射物的Socket的位置
	virtual void die()override;
	UAnimMontage* GetHitReactMontage_Implementation();
	bool IsDead_Implementation();
	AActor* GetAvatar_Implementation();
	TArray<FTaggedMontage>GetAttackMontages_Implementation();
	UNiagaraSystem*GetBloodEffect_Implementation();
	FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag);
	int32 GetMinionCount_Implementation();
	void IncrementalMinionCount_Implementation(int32 Amount);
	/*CombatInterface*/


	/*Dissolve Effect*/

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr <UMaterialInstance>DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr <UMaterialInstance>WeaponDissolveMaterialInstance;

	/*Dissolve Effect*/


	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray <FTaggedMontage> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSound;


protected:
	bool bIsDead = false;


private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>>StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage>HitReactMontage;
};
