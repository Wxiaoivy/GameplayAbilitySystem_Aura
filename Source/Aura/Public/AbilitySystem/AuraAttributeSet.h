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
//ATTRIBUTE_ACCESSORS �����ڶ���һ�����Ե� getter �� setter ������
// ����һ����ͨ�õĺ꣬���������κ���Ҫ�Զ��� getter �� setter �ĳ���(����Initter)���������� Gameplay Attributes��
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
     GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
     GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
     GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
     GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

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
	//GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAuraAttributeSet, Health); ���ֻ��Getter��������ATTRIBUTE_ACCESSORS����
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	

	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

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
