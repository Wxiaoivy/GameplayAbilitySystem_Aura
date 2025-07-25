// Copyright Ivy


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

    DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance)
    DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance)
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance)
    DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance)

   

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

       

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

    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}


void UExecCalc_Damage::DeterminDebuff(const FGameplayEffectSpec& Spec, const FGameplayEffectCustomExecutionParameters& ExecutionParams, TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToCaptureDefs, FAggregatorEvaluateParameters& EvaluateParameters) const
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();//存储项目中所有的GameplayTag（如伤害类型、Debuff类型等）。
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToDebuff)//遍历所有可能的伤害类型，检查是否应该触发对应的 Debuff。
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);//从技能效果（GameplayEffectSpec）中获取指定Tag（如火焰伤害）的数值。这里获得不同伤害类型的伤害值,
		if (TypeDamage > -1.f)       //如果返回值 > -1，说明当前伤害类型有效,  如果返回 -1.f，表示该伤害类型未生效，跳过后续逻辑。
		{
			const float SourceDebuffChange = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);//施法者的基础Debuff触发几率（从技能配置中读取）。

			float TargetDebuffResistance = 0.f;
			const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistance[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToCaptureDefs[ResistanceTag], EvaluateParameters, TargetDebuffResistance);//从目标的属性系统中捕获抗性值
			TargetDebuffResistance = FMath::Max<float>(0, TargetDebuffResistance);//确保抗性值不会为负数（最小为 0）
			const float EffectDebuffChance = SourceDebuffChange * (100 -0/*TargetDebuffResistance*/) / 100;//最终概率 = 施法者基础概率 × (100 - 目标抗性) / 100


			bool bDebuff = FMath::RandRange(1, 100) < EffectDebuffChance;
			if (bDebuff)
			{
                //设置AuraAbilityTypes里面的FAuraGameplayEffectContext的各个变量的值。往Context里面补充Debuff信息,接下来就可以在AuraAttributeSet里面应用GE了。
                FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

                UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(EffectContextHandle, true);//这是原始伤害的Context,设置了IsSuccessfulDebuff，但是动态创建的DebuffEffectContext没有设置。
                UAuraAbilitySystemLibrary::SetDamageType(EffectContextHandle, DamageType);
                //FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect（）这个函数里面已经用了AssignTagSetByCallerMagnitude把Tag和值绑定配置在一起了。
                const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
                const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
                const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);

                UAuraAbilitySystemLibrary::SetDebuffDamage(EffectContextHandle, DebuffDamage);
                UAuraAbilitySystemLibrary::SetDebuffDuration(EffectContextHandle, DebuffDuration);
                UAuraAbilitySystemLibrary::SetDebuffFrequency(EffectContextHandle, DebuffFrequency);
                //设置AuraAbilityTypes里面的FAuraGameplayEffectContext的各个变量的值。往Context里面补充Debuff信息，接下来就可以在AuraAttributeSet里面应用GE了。
			}
		}
	}
}


void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

    TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	TagsToCaptureDefs.Add(Tags.Attribute_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attribute_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(Tags.Attribute_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);
	TagsToCaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);


	TagsToCaptureDefs.Add(Tags.Attribute_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attribute_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attribute_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attribute_Resistance_Physical, DamageStatics().PhysicalResistanceDef);

    /*SetBaseInfo*/
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}

	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
   // ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    /*SetBaseInfo*/


    /*FAggregatorEvaluateParameters EvaluateParameters;*/
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;
    /*FAggregatorEvaluateParameters* EvaluateParameters;*/

    //是否实现Debuff
    DeterminDebuff(Spec, ExecutionParams, TagsToCaptureDefs, EvaluateParameters);


    //通过SetByCaller得到动态的Damage
    float Damage = 0.f;
    for (const auto& Pair:FAuraGameplayTags::Get().DamageTypesToResistance)
    {
        const FGameplayTag DamageTypeTag = Pair.Key;
        const FGameplayTag ResistanceTag = Pair.Value;
        FGameplayEffectAttributeCaptureDefinition CaptureDef;

        if (TagsToCaptureDefs.Contains(ResistanceTag))
        {
           CaptureDef = TagsToCaptureDefs[ResistanceTag];
        }


        float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key,false);//通过相同的GameplayTag查找并获取之前设置的动态数值（在AuraProjectileSpell.cpp的AssignTagSetByCallerMagnitude里设置好了）
       
        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

        DamageTypeValue *= (100.f - Resistance) / 100.f;
    
        Damage += DamageTypeValue;
    }




    /*捕获BlockChance的值，如果成功Block则伤害减半*/  //（下面的这三行就是得到Attribute里面各属性的值的模板）
    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);//这里的TargetBlockChance是输出参数
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0);

    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

    FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
    UAuraAbilitySystemLibrary::SetIsBlockedHit(ContextHandle, bBlocked);//传递bBlocked，到蓝图里调用

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
    float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

    FRealCurve* EffectArmorCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("EffectArmor"), FString());
    float EffectArmorCoefficient = ArmorPenetrationCurve->Eval(TargetPlayerLevel);
    //得到CurveTable中的各等级的Coefficient（ArmorPenetrationCoefficient，EffectArmorCoefficient）


    //得到EffectArmor（Target的有效护甲），该变量是等于Tatget的护甲减去（Source的护甲穿透乘以一个系数）
    const float EffectArmor = TargetArmor *= (100 - SourceArmorPenetration* ArmorPenetrationCoefficient) / 100;


    //算出Damage，有效护甲会抵抗伤害（抵抗的伤害是以 有效护甲*一个系数 来计算的）
    Damage *= (100 - EffectArmor* EffectArmorCoefficient) / 100;


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
    float CriticalHitResistanceCoefficient = ArmorPenetrationCurve->Eval(TargetPlayerLevel);
    //得到CurveTable中的各等级的Coefficient(CriticalHitResistanceCoefficient)


    //得到有效的暴击率（本人暴击率-敌军暴击抵抗*系数）
    const float EffectCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;


    //确定是否暴击，如果暴击的话 伤害就等于2倍伤害+CriticalHitDamage
    bool bCriticalHit = FMath::RandRange(1, 100) < EffectCriticalHitChance;
    UAuraAbilitySystemLibrary::SetIsCriticalHit(ContextHandle, bCriticalHit);//传递bCriticalHit,到蓝图里调用
    Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;


    const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);


}

