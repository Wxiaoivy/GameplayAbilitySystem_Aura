// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AuraAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();

	// 重写父类的GetLifetimeReplicatedProps函数，用于指定哪些属性需要在网络上复制
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	
	// 定义一个名为Health的属性，它是FGameplayAttributeData类型的
	// BlueprintReadOnly表示这个属性可以在蓝图（Blueprint）中读取，但不能修改
	// ReplicatedUsing=OnRep_Health表示这个属性的复制会调用OnRep_Health函数
	// Category="Vital Attributes"表示这个属性在编辑器中的分类是“Vital Attributes”
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Health,Category="Vital Attributes")
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;

	// 定义一个名为OnRep_Health的函数，当Health属性在网络上被复制时会调用这个函数
	// OldHealth参数表示复制前的Health属性值
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldHealth) const;

};
