// Copyright Ivy


#include "Player/AuraPlayerState.h"

AAuraPlayerState::AAuraPlayerState()
{
	//��һ����������»�����ͬ����ص�Ƶ������
	//���ڿͻ��˺ͷ�����֮�������ͬ��
	NetUpdateFrequency = 100.0f;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
}

void AAuraPlayerState::OnRep_Level( int32 OldLevel) 
{

}

int32 AAuraPlayerState::GetPlayerLevel()const
{
	return Level;
}
