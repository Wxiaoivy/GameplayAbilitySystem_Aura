﻿// Copyright Ivy


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance)
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	//这段代码使用 静态函数 + 静态局部变量 的方式实现了一个 单例模式（Singleton Pattern），目的是 全局唯一地获取 AuraDamageStatics 实例。
    //🎯 设计目的
    //1. 全局唯一访问点
    //确保整个程序中 只有一个 AuraDamageStatics 实例，避免重复创建造成资源浪费。
    //
    //任何地方调用 DamageStatics() 都返回同一个对象。
    //
    //2. 延迟初始化（Lazy Initialization）
    //只有 第一次调用 DamageStatics() 时才会构造 DStatics，避免程序启动时的冗余初始化。
    //
    //3. 线程安全（C++11 起）
    //C++11 规定 静态局部变量的初始化是线程安全的，因此无需额外加锁。
    //
    //4. 隐藏实现细节
    //将单例实例 DStatics 封装在函数内部，避免全局变量污染命名空间。
	static AuraDamageStatics DStatics;
	return DStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
    RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    /*SetBaseInfo*/
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    ICombatInterface* SourceCombatInterface = Cast< ICombatInterface>(SourceAvatar);
    ICombatInterface* TargetCombatInterface = Cast< ICombatInterface>(TargetAvatar);

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    /*SetBaseInfo*/


    //得到原始Damage
    float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);


    /*FAggregatorEvaluateParameters EvaluateParameters;*/
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;
    /*FAggregatorEvaluateParameters* EvaluateParameters;*/


    /*捕获BlockChance的值，如果成功Block则伤害减半*/  //（下面的这三行就是得到Attribute里面各属性的值的模板）
    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);//这里的TargetBlockChance是输出参数
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0);

    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
    Damage = bBlocked ? Damage / 2 : Damage;
    /*已经捕获BlockChance，如果成功Block则伤害减半*/


    /*捕获Armor和ArmorPenetration的值*/
    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
    TargetArmor = FMath::Max<float>(TargetArmor, 0);

    float SourceArmorPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0);
    /*捕获Armor和ArmorPenetration的值*/


    //获得CharacterClassInfo
    UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);


    //得到CurveTable中的各等级的Coefficient（下面两行是模板）（ArmorPenetrationCoefficient，EffectArmorCoefficient）
    FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("ArmorPenetration"), FString());
    float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

    FRealCurve* EffectArmorCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("EffectArmor"), FString());
    float EffectArmorCoefficient = ArmorPenetrationCurve->Eval(TargetCombatInterface->GetPlayerLevel());
    //得到CurveTable中的各等级的Coefficient（ArmorPenetrationCoefficient，EffectArmorCoefficient）


    //得到EffectArmor（Target的有效护甲），该变量是等于Tatget的护甲减去（Source的护甲穿透乘以一个系数）
    const float EffectArmor = TargetArmor *= (100 - SourceArmorPenetration* 0.25) / 100;


    //算出Damage，有效护甲会抵抗伤害（抵抗的伤害是以 有效护甲*一个系数 来计算的）
    Damage *= (100 - EffectArmor* 0.33) / 100;


    /*捕获CriticalHitChance和CriticalHitDamage和CriticalHitResistance的值*/
    float SourceCriticalHitChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
    SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0);

    float SourceCriticalHitDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);
    SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0);

    float TargetCriticalHitResistance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);
    TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0);

    /*捕获CriticalHitChance和CriticalHitDamage和CriticalHitResistance的值*/


     //得到CurveTable中的各等级的Coefficient(CriticalHitResistanceCoefficient)
    FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("CriticalHitResistance"), FString());
    float CriticalHitResistanceCoefficient = ArmorPenetrationCurve->Eval(TargetCombatInterface->GetPlayerLevel());
    //得到CurveTable中的各等级的Coefficient(CriticalHitResistanceCoefficient)


    //得到有效的暴击率（本人暴击率-敌军宝鸡抵抗*系数）
    const float EffectCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;


    //确定是否暴击，如果暴击的话 伤害就等于2倍伤害+CriticalHitDamage
    bool bCriticalHit = FMath::RandRange(1, 100) < EffectCriticalHitChance;
    Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;


    const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);


}
