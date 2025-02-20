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
	// ��Դ������Ч���Ķ��󣩺�Ŀ�꣨Ч��Ӧ�õĶ������ռ���ǩ��
	// SourceTag �洢Դ����ı�ǩ���ϣ�TargetTag �洢Ŀ�����ı�ǩ���ϡ�
	const FGameplayTagContainer* SourceTag = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTag = Spec.CapturedTargetTags.GetAggregatedTags();

	// ����һ�� FAggregatorEvaluateParameters ���͵ı��� EvaluateParameters�����ں����������Է���ʱ����Դ��Ŀ���ǩ��
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTag;
	EvaluateParameters.TargetTags = TargetTag;

	// ����һ�� float ���͵ı��� Int�����ڴ洢��Դ��Ŀ���ǩ�м�����Ļ���ֵ��һ�����ԣ���
	float Int = 0.f;

	// ���� GetCapturedAttributeMagnitude ���������ݶ�������Զ��壨IntDef�����ṩ�Ĳ������������ֵ�����洢�� Int �����С�
	// ȷ�� Int ��ֵ��С�� 0��
	GetCapturedAttributeMagnitude(IntDef, Spec, EvaluateParameters, Int);
	Int = FMath::Max<float>(Int, 0.f);

	// ���Խ� Spec ���������е�Դ����ת��Ϊ ICombatInterface �ӿ�ָ�롣
	// ICombatInterface ��һ���ӿڣ������ṩս����صĹ��ܣ������ȡ��ҵȼ���
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	//�Զ���MaxMana��ʽ�������ظü����ֵ
	float MaxMana = 50.f + 2.5 * Int + 15.f * PlayerLevel;
	return MaxMana;
}
