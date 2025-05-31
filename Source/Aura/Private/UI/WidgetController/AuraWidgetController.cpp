// Copyright Ivy


#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerController.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;

}

void UAuraWidgetController::BroadCastInitialValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraASC()->bStartupAbilitiesGiven)return;// ȷ�������ѳ�ʼ��


	// ����Lambdaί�У�����ÿ������
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda
	([this](const FGameplayAbilitySpec& AbilitySpec)
		{
			// 1. ���ݼ��ܱ�ǩ����UI�������Ϣ����ͼ�ꡢ������
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(GetAuraASC()->GetAbilityTagFormSpec(AbilitySpec));
			// 2. �������ǩ
			Info.InputTag = GetAuraASC()->GetInputTagFormSpec(AbilitySpec);
			// 3. �㲥��UI����
			AbilityInfoDelegate.Broadcast(Info);

		}
	);
	// �������м��ܲ�ִ��ί��
	GetAuraASC()->ForEachAbility(BroadcastDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	if (AuraPlayerController == nullptr)
	{
		AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
	}
	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	if (AuraPlayerState==nullptr)
	{
		AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
	}
	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
	if (AuraAbilitySystemComponent==nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
	if (AuraAttributeSet==nullptr)
	{
		AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	}
	return AuraAttributeSet;
}

