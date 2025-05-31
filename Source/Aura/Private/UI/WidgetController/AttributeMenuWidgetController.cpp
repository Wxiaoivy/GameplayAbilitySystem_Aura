// Copyright Ivy


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadCastInitialValues()
{
	
	check(AttributeInfo);
	//���� AS->TagsToAttributes �еļ�ֵ��,��������Pair�С�
	for (auto& Pair : GetAuraAS()->TagsToAttributes)
	{
		//FAuraAttributeInfo��AttributeInfo���еĽṹ��
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
		//�����Pair.Value�Ǹ���̬����ָ�룬ͨ������ָ����ú���ʱ����Ҫ�ں���ָ������ ()�������ݲ���������û�в������ô��Ρ�
		//Pair.Value:����һ����̬����ָ�룬������ TStaticFuncPtr<FGameplayAttribute()>���� FGameplayAttribute(*)()��
        // ��ָ��һ������ FGameplayAttribute ��û�в����ľ�̬������
		// Pair.Value() :ͨ������ָ����ú���������һ�� FGameplayAttribute ����
		// GetNumericValue(AS) :���� FGameplayAttribute ��ĳ�Ա���������ڻ�ȡ���Ե���ֵ��
		Info.AttributeValue = Pair.Value().GetNumericValue(AuraAttributeSet);
		//����㲥����Info
		AttributeInfoDelegate.Broadcast(Info);
	}
	

	
	OnAttributePointsChangedDelegateInController.Broadcast(AuraPlayerState->GetAttributePoints());
	//OnAttributePointsChangedDelegateInController.Clear();//���������Լ���AI�ӵ� ��Ϊÿ�ΰ��˵���ť��ʼ��ʱ  �ڶ��λ�ִ��2�δ���  �����λ�ִ��3�δ����������ֹ�ظ��󶨡�(���ܼ����в�Ȼ�������)
}

	void UAttributeMenuWidgetController::BindCallbacksToDependencies()
	{
		//UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
		check(AttributeInfo);
		for (auto& Pair : GetAuraAS()->TagsToAttributes)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
				[this,Pair](const FOnAttributeChangeData& Data)//��ȻData��������û���� �������GetGameplayAttributeValueChangeDelegateҪ�󱻰󶨵ĺ����������������
				{
					FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
					Info.AttributeValue = Pair.Value().GetNumericValue(GetAuraAS());

					AttributeInfoDelegate.Broadcast(Info);
				}
			);
		}
		//AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
		GetAuraPS()->OnAttirbutePointsChangedDelegate.AddLambda
		([this](int32 InPoints)
			{
				OnAttributePointsChangedDelegateInController.Broadcast(InPoints);
			}
		);
	}

	void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
	{
	     GetAuraASC()->UpgradeAttribute(AttributeTag);
	}
