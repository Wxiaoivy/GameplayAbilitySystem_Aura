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
	// ��Դ������Ч���Ķ��󣩺�Ŀ�꣨Ч��Ӧ�õĶ������ռ���ǩ��
	// SourceTag �洢Դ����ı�ǩ���ϣ�TargetTag �洢Ŀ�����ı�ǩ���ϡ�
	const FGameplayTagContainer* SourceTag = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTag = Spec.CapturedTargetTags.GetAggregatedTags();

	// ����һ�� FAggregatorEvaluateParameters ���͵ı��� EvaluateParameters�����ں����������Է���ʱ����Դ��Ŀ���ǩ��
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTag;
	EvaluateParameters.TargetTags = TargetTag;

	// ����һ�� float ���͵ı��� Vigor�����ڴ洢��Դ��Ŀ���ǩ�м�����Ļ���ֵ��һ�����ԣ���
	float Vigor = 0.f;

	// ���� GetCapturedAttributeMagnitude ���������ݶ�������Զ��壨VigorDef�����ṩ�Ĳ������������ֵ�����洢�� Vigor �����С�
	// ȷ�� Vigor ��ֵ��С�� 0��
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	// ���Խ� Spec ���������е�Դ����ת��Ϊ ICombatInterface �ӿ�ָ�롣
    // ICombatInterface ��һ���ӿڣ������ṩս����صĹ��ܣ������ȡ��ҵȼ���
	int32 PlayerLevel = 0;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	

	//�Զ���MaxHealth��ʽ�������ظü����ֵ
	float MaxHealth= 80.f + 2.5 * Vigor + 10.f * PlayerLevel;
	return MaxHealth;

}
