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
    FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
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
    // 遍历所有伤害类型与抗性的映射关系（如FireDamage -> FireResistance）
    for (const auto& Pair:FAuraGameplayTags::Get().DamageTypesToResistance)
    {
        // 获取当前伤害类型标签（如FireDamage）
        const FGameplayTag DamageTypeTag = Pair.Key;

        // 获取对应的抗性标签（如FireResistance）
        const FGameplayTag ResistanceTag = Pair.Value;

        // 声明抗性属性的捕获定义（用于从目标的AttributeSet中读取抗性值）
        FGameplayEffectAttributeCaptureDefinition CaptureDef;

        // 检查抗性标签是否已注册到捕获定义表中
        if (TagsToCaptureDefs.Contains(ResistanceTag))
        {
            // 获取该抗性标签对应的属性捕获定义（如FireResistance属性）
           CaptureDef = TagsToCaptureDefs[ResistanceTag];
        }

        // 通过SetByCaller机制获取当前伤害类型的初始值（如FireDamage = 100）
        float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key,false);//通过相同的GameplayTag查找并获取之前设置的动态数值（在AuraProjectileSpell.cpp的AssignTagSetByCallerMagnitude里设置好了）
        if (DamageTypeValue <= 0) //DamageTypeValue <= 0.f: 跳过未配置或强制无效的伤害类型！它的跳过仅基于技能配置，而非目标是否在受到伤害。
		{
            continue;// 直接进入下一循环迭代


			/*	(1) 场景1：技能仅配置火焰伤害
				遍历时的行为：
					火焰伤害：DamageTypeValue = 100 → 不跳过，计算抗性并累加。
					其他伤害类型：DamageTypeValue = 0（未配置）→ 触发 continue，跳过计算。
					结果：只有火焰伤害生效，其他类型被过滤。*/

            //这个项目每个技能只配置了一个类型。 所以每次遍历到的别的类型就会直接  DamageTypeValue <= 0 并跳过这次循环
       }

        // 初始化抗性值为0（默认无抗性）
        float Resistance = 0.f;
        // 从目标的AttributeSet中捕获实际抗性值（如FireResistance=30）
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
        // 钳制抗性值在0~100范围内（避免负数或超100%抗性）
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

        // 计算抗性减免后的实际伤害（如100 * (1 - 0.3) = 70）
        DamageTypeValue *= (100.f - Resistance) / 100.f;

        // 检查是否为范围伤害（通过EffectContext中的标记判断）
		if (UAuraAbilitySystemLibrary::IsRadialDamage(ContextHandle))
		{
            // 如果目标实现了战斗接口（ICombatInterface）
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
			{
                // 绑定Lambda到伤害代理，用于动态更新DamageTypeValue
				CombatInterface->GetOnDamageDelegete().AddLambda
				(
					[&](float DamageAmount)
					{
                        DamageTypeValue = DamageAmount; // 接收TakeDamage广播的实际伤害
                    }
				);
			}
            // 调用引擎函数施加范围伤害（带衰减）
			UGameplayStatics::ApplyRadialDamageWithFalloff
            (
				TargetAvatar,        // 目标Actor（通常为伤害中心点）
				DamageTypeValue,     // 基础伤害值（可能被代理更新）
				0.f,                 // 最小伤害（最外圈的伤害值）
				UAuraAbilitySystemLibrary::GetRadialDamageOrigin(ContextHandle),// 伤害中心坐标
                UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(ContextHandle),// 内圈半径（满伤害区域）
                UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(ContextHandle),// 外圈半径（伤害衰减到0）
				1.f,                       // 衰减曲线指数（1.0 = 线性衰减）
				UDamageType::StaticClass(),// 伤害类型（可自定义子类）
				TArray<AActor*>(),         // 忽略的Actor列表
				SourceAvatar,              // 伤害来源Actor
				nullptr                    // 伤害控制器（可空）
            );                  
		}
		

        Damage += DamageTypeValue;// 将当前伤害类型的值累加到总伤害（非范围伤害时使用）
    }




    /*捕获BlockChance的值，如果成功Block则伤害减半*/  //（下面的这三行就是得到Attribute里面各属性的值的模板）
    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);//这里的TargetBlockChance是输出参数
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0);

    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

   
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

