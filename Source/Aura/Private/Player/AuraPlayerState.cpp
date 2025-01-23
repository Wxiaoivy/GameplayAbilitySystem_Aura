// Copyright Ivy


#include "Player/AuraPlayerState.h"

AAuraPlayerState::AAuraPlayerState()
{
	//��һ����������»�����ͬ����ص�Ƶ������
	//���ڿͻ��˺ͷ�����֮�������ͬ��
	NetUpdateFrequency = 100.0f;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
