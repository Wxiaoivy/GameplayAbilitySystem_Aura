// Copyright Ivy


#include "AbilitySystem/MMC/MMC_MaxHealth.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);

}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
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

	// 定义一个 float 类型的变量 Vigor，用于存储从源和目标标签中计算出的活力值（一种属性）。
	float Vigor = 0.f;

	// 调用 GetCapturedAttributeMagnitude 函数，根据定义的属性定义（VigorDef）和提供的参数，计算活力值，并存储在 Vigor 变量中。
	// 确保 Vigor 的值不小于 0。
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	// 尝试将 Spec 的上下文中的源对象转换为 ICombatInterface 接口指针。
    // ICombatInterface 是一个接口，用于提供战斗相关的功能，比如获取玩家等级。
	int32 PlayerLevel = 0;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	

	//自定义MaxHealth算式，并返回该计算的值
	float MaxHealth= 80.f + 2.5 * Vigor + 10.f * PlayerLevel;
	return MaxHealth;

}
