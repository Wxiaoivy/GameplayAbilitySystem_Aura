// Copyright Ivy


#include "AbilitySystem/MMC/MMC_MaxMana.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//Gather tags from source and target 
	// 从源（触发效果的对象）和目标（效果应用的对象）上收集标签。
	// SourceTag 存储源对象的标签集合，TargetTag 存储目标对象的标签集合。
	const FGameplayTagContainer* SourceTag = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTag = Spec.CapturedTargetTags.GetAggregatedTags();

	// 创建一个 FAggregatorEvaluateParameters 类型的变量 EvaluateParameters，用于后续评估属性幅度时传递源和目标标签。
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTag;
	EvaluateParameters.TargetTags = TargetTag;

	// 定义一个 float 类型的变量 Int，用于存储从源和目标标签中计算出的活力值（一种属性）。
	float Int = 0.f;

	// 调用 GetCapturedAttributeMagnitude 函数，根据定义的属性定义（IntDef）和提供的参数，计算活力值，并存储在 Int 变量中。
	// 确保 Int 的值不小于 0。
	GetCapturedAttributeMagnitude(IntDef, Spec, EvaluateParameters, Int);
	Int = FMath::Max<float>(Int, 0.f);

	// 尝试将 Spec 的上下文中的源对象转换为 ICombatInterface 接口指针。
	// ICombatInterface 是一个接口，用于提供战斗相关的功能，比如获取玩家等级。
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	//自定义MaxMana算式，并返回该计算的值
	float MaxMana = 50.f + 2.5 * Int + 15.f * PlayerLevel;
	return MaxMana;
}
