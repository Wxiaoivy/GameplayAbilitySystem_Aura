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

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject>WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect>DamageGameplayEffectClass;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent>SourceAbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent>TargetAbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DeathImpuseMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpuse = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	float KnockBackMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float KnockBackChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector KnockBackForce = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;

	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;

};



USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

protected:

	/*�����Լ��Զ�����ӵı�����������EffectContext�в�����Ϣ*/
	UPROPERTY()
	bool bIsBlockHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
	/*�����Լ��Զ�����ӵı�����������EffectContext�в�����Ϣ*/

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;

	TSharedPtr<FGameplayTag>DamageType;

	UPROPERTY()
	FVector DeathImpuse = FVector::ZeroVector;

	UPROPERTY()
	FVector KnockBackForce = FVector::ZeroVector;

	UPROPERTY()
	bool bIsRadialDamage = false;

	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;

public:

	bool IsCriticalHit()const { return bIsCriticalHit; }
	bool IsBlockHit()const { return bIsBlockHit; }
	bool IsSuccessfulDebuff()const { return bIsSuccessfulDebuff; }
	float GetDebuffDamage()const { return DebuffDamage; }
	float GetDebuffDuration()const { return DebuffDuration; }
	float GetDebuffFrequency()const { return DebuffFrequency; }
	TSharedPtr<FGameplayTag> GetDamageType()const { return DamageType; }
	FVector GetDeathImpuse()const { return DeathImpuse; }
	FVector GetKnockBackForce()const { return KnockBackForce; }
	bool IsRadialDamage()const { return bIsRadialDamage; }
	float GetRadialDamageInnerRadius()const { return RadialDamageInnerRadius; }
	float GetRadialDamageOuterRadius()const {return RadialDamageOuterRadius;}
	FVector GetRadialDamageOrigin()const { return RadialDamageOrigin; }


	void SetIsCriticalHit(bool InIsCriticalHit) {bIsCriticalHit = InIsCriticalHit;}
	void SetIsBlockedHit(bool InIsBlockedHit) { bIsBlockHit = InIsBlockedHit; }
	void SetIsSuccessfulDebuff(bool InIsSuccessfulDebuff) { bIsSuccessfulDebuff = InIsSuccessfulDebuff; }
	void SetDebuffDamage(float InDebuffDamage) { DebuffDamage = InDebuffDamage; }
	void SetDebuffDuration(float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	void SetDebuffFrequency(float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
	void SetDeathImpuse(const FVector& InImpuse) { DeathImpuse = InImpuse; }
	void SetKnockBackForce(const FVector& InKnockBackForce) { KnockBackForce = InKnockBackForce; }
	void SetIsRadialDamage(bool InIsRadialDamage) { bIsRadialDamage = InIsRadialDamage; }
	void SetRadialDamageInnerRadius(float InRadialDamageInnerRadius) { RadialDamageInnerRadius = InRadialDamageInnerRadius; }
	void SetRadialDamageOuterRadius(float InRadialDamageOuterRadius) { RadialDamageOuterRadius = InRadialDamageOuterRadius; }
	void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin) { RadialDamageOrigin = InRadialDamageOrigin; }

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