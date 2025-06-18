// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY()
	TObjectPtr<UObject>WorldContextObject = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect>DamageGameplayEffectClass;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>SourceAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>TargetAbilitySystemComponent;

	UPROPERTY()
	float BaseDamage = 0.f;

	UPROPERTY()
	float AbilityLevel = 1.f;

	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	float DebuffChance = 0.f;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;


};



USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	bool IsCriticalHit()const { return bIsCriticalHit; }
	bool IsBlockHit()const { return bIsBlockHit; }
	//bool IsSuccessfulDebuff()const { return bIsSuccessfulDebuff; }
	//float GetDebuffDamage()const { return DebuffDamage; }
	//float GetDebuffDuration()const { return DebuffDuration; }
	//float GetDebuffFrequency()const { return DebuffFrequency; }
	//TSharedPtr<FGameplayTag> GetDamageType()const { return DamageType; }


	void SetIsCriticalHit(bool InIsCriticalHit) {bIsCriticalHit = InIsCriticalHit;}
	void SetIsBlockedHit(bool InIsBlockedHit) { bIsBlockHit = InIsBlockedHit; }
	//void SetIsSuccessfulDebuff(bool InIsSuccessfulDebuff) { bIsSuccessfulDebuff = InIsSuccessfulDebuff; }
	//void SetDebuffDamage(float InDebuffDamage) { DebuffDamage = InDebuffDamage; }
	//void SetDebuffDuration(float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	//void SetDebuffFrequency(float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	//void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
	//


	//��д�ĺ�����һ�� GetScriptStruct()
	/** Returns the actual struct used for serialization, subclasses must override this! */
	//������಻��д������� GetScriptStruct() �᷵�� FGameplayEffectContext::StaticStruct()���������л�ʱ��ʧ���������ĳ�Ա���� bIsCriticalHit �� bIsBlockHit����
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FAuraGameplayEffectContext::StaticStruct(); // ������������� Struct ����
	}



	//��д�ĺ��������� Duplicate()
	/** Creates a copy of this context, used to duplicate for later modifications */
	//�������д������� Duplicate() ֻ�´�������Ա�����������ı������� bIsCriticalHit���ᱻ��ʧ��
	virtual  FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new  FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}


	//��д�ĺ��������� NetSerialize
	/** Custom serialization, subclasses must override this */
	//���������ı������� bIsBlockHit�������ֶ����л������򲻻ᱻͬ�����ͻ��ˡ������ NetSerialize ���ᴦ�������Ա��
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;


protected:

	/*�����Լ��Զ�����ӵı�����������EffectContext�в�����Ϣ*/
	UPROPERTY()
	bool bIsBlockHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
	/*�����Լ��Զ�����ӵı�����������EffectContext�в�����Ϣ*/

	//UPROPERTY()
	//bool bIsSuccessfulDebuff = false;

	//UPROPERTY()
	//float DebuffDamage = 0.f;

	//UPROPERTY()
	//float DebuffFrequency = 0.f;

	//UPROPERTY()
	//float DebuffDuration = 0.f;

	//TSharedPtr<FGameplayTag>DamageType;


};

//��д�Ľṹ�壨�ģ�  TStructOpsTypeTraits
//Unreal Engine �� USTRUCT Ĭ�ϲ�֧��ĳЩ�߼�������������ͬ�������������Ҫͨ�� TStructOpsTypeTraits ��ʽ����֧�ֵĹ��ܡ�
//��� FAuraGameplayEffectContext �̳��� FGameplayEffectContext������������Ѿ�����ʵ������Щ���ܣ������������ĳ�Ա���� bIsCriticalHit��bIsBlockHit����Ҫ�������á�
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> :public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,      //�������л���WithNetSerializer��
		WithCopy = true          //����

	};
};