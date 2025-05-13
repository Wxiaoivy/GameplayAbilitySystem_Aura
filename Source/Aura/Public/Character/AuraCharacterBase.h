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

	

	

	//NetMulticast����ʾ����һ�� �ಥ RPC�����ڷ��������ú�ͬ�������пͻ��ˡ�
	//Reliable����֤�� RPC �ض���ִ�У���ʹ���������Ҳ������ֱ���ɹ�����
	/*�ؼ��㣺

		�����ڷ��������á�

		ʹ�� _Implementation ʵ�ֺ�����

		�� Reliable ȷ���ؼ��߼�����ʧ��*/
	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//��AbilitySystem����һ������һ�������֣�����е�ӣ�
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

	//Unreal Header Tool(UHT) ���Զ��������´��룺
	//	һ�����麯�� GetCombatSocketLocation()�������� C++ ����ʵ�֣���
	//	һ��Ĭ��ʵ�� GetCombatSocketLocation_Implementation()����ѡ���ǣ���
	//	һ��ȫ��ִ�к��� Execute_GetCombatSocketLocation(UObject * Object)�����ڶ�̬���ã���
	FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag);//�õ��������淢���������Socket��λ��
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
