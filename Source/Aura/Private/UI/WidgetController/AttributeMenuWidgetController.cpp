// Copyright Ivy


#include "UI/WidgetController/AttributeMenuWidgetController.h"

void UAttributeMenuWidgetController::BroadCastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	//
	check(AttributeInfo);
	//���� AS->TagsToAttributes �еļ�ֵ��,��������Pair�С�
	for (auto& Pair : AS->TagsToAttributes)
	{
		//FAuraAttributeInfo��AttributeInfo���еĽṹ��
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
		//�����Pair.Value�Ǹ���̬����ָ�룬ͨ������ָ����ú���ʱ����Ҫ�ں���ָ������ ()�������ݲ���������û�в������ô��Ρ�
		//Pair.Value:����һ����̬����ָ�룬������ TStaticFuncPtr<FGameplayAttribute()>���� FGameplayAttribute(*)()��
        // ��ָ��һ������ FGameplayAttribute ��û�в����ľ�̬������
		// Pair.Value() :ͨ������ָ����ú���������һ�� FGameplayAttribute ����
		// GetNumericValue(AS) :���� FGameplayAttribute ��ĳ�Ա���������ڻ�ȡ���Ե���ֵ��
		Info.AttributeValue = Pair.Value().GetNumericValue(AS);
		//����㲥����Info
		AttributeInfoDelegate.Broadcast(Info);
	}
}

	void UAttributeMenuWidgetController::BindCallbacksToDependencies()
	{
		UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
		check(AttributeInfo);
		for (auto& Pair : AS->TagsToAttributes)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
				[this,Pair,AS](const FOnAttributeChangeData& Data)//��ȻData��������û���� �������GetGameplayAttributeValueChangeDelegateҪ�󱻰󶨵ĺ����������������
				{
					FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
					Info.AttributeValue = Pair.Value().GetNumericValue(AS);

					AttributeInfoDelegate.Broadcast(Info);
				}
			);
		}
	}
