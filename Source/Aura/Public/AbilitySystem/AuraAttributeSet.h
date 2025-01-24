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

	// ��д�����GetLifetimeReplicatedProps����������ָ����Щ������Ҫ�������ϸ���
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	
	// ����һ����ΪHealth�����ԣ�����FGameplayAttributeData���͵�
	// BlueprintReadOnly��ʾ������Կ�������ͼ��Blueprint���ж�ȡ���������޸�
	// ReplicatedUsing=OnRep_Health��ʾ������Եĸ��ƻ����OnRep_Health����
	// Category="Vital Attributes"��ʾ��������ڱ༭���еķ����ǡ�Vital Attributes��
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Health,Category="Vital Attributes")
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;

	// ����һ����ΪOnRep_Health�ĺ�������Health�����������ϱ�����ʱ������������
	// OldHealth������ʾ����ǰ��Health����ֵ
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldHealth) const;

};
