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

	//得到武器上面发射抛射物的Socket的位置
	virtual FVector GetCombatSocketLocation()override;

	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffect, float level)const;
	virtual void InitializeDefualtAttributes()const;

	void AddCharacterAbilities();

 public:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent>Weapon;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr <UAttributeSet>AttributeSet;

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>>StartupAbilities;

	
};
