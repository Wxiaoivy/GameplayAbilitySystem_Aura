// Copyright Ivy


#include "Player/AuraPlayerState.h"

AAuraPlayerState::AAuraPlayerState()
{
	//是一个与网络更新或数据同步相关的频率设置
	//用于客户端和服务器之间的数据同步
	NetUpdateFrequency = 100.0f;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
