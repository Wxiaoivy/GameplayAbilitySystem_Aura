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

	/*这是自己自定义添加的变量，用于在EffectContext中补充信息*/
	UPROPERTY()
	bool bIsBlockHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
	/*这是自己自定义添加的变量，用于在EffectContext中补充信息*/

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

	//重写的函数（一） GetScriptStruct()
	/** Returns the actual struct used for serialization, subclasses must override this! */
	//如果子类不重写，父类的 GetScriptStruct() 会返回 FGameplayEffectContext::StaticStruct()，导致序列化时丢失子类新增的成员（如 bIsCriticalHit 和 bIsBlockHit）。
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FAuraGameplayEffectContext::StaticStruct(); // 返回子类自身的 Struct 类型
	}



	//重写的函数（二） Duplicate()
	/** Creates a copy of this context, used to duplicate for later modifications */
	//如果不重写，父类的 Duplicate() 只会拷贝父类成员，子类新增的变量（如 bIsCriticalHit）会被丢失。
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


	//重写的函数（三） NetSerialize
	/** Custom serialization, subclasses must override this */
	//子类新增的变量（如 bIsBlockHit）必须手动序列化，否则不会被同步到客户端。父类的 NetSerialize 不会处理子类成员。
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;




};

//重写的结构体（四）  TStructOpsTypeTraits
//Unreal Engine 的 USTRUCT 默认不支持某些高级操作（如网络同步、深拷贝），需要通过 TStructOpsTypeTraits 显式声明支持的功能。
//你的 FAuraGameplayEffectContext 继承自 FGameplayEffectContext，而父类可能已经部分实现了这些功能，但子类新增的成员（如 bIsCriticalHit、bIsBlockHit）需要额外配置。
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> :public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,      //网络序列化（WithNetSerializer）
		WithCopy = true          //拷贝

	};
};