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


protected:

	/*这是自己自定义添加的变量，用于在EffectContext中补充信息*/
	UPROPERTY()
	bool bIsBlockHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
	/*这是自己自定义添加的变量，用于在EffectContext中补充信息*/

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