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
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}


void AAuraPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level,true); // ʹ�� true / false ���������������ʹ浵���������ֲ�ͬ�ĵȼ��仯����� ���AddToLevel����ֻ�������������������
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level,false);//ʹ�� true/false ���������������ʹ浵���������ֲ�ͬ�ĵȼ��仯����� ���SetLevel����ֻ���ڴ浵��������ŵ���
}

void AAuraPlayerState::SetAttributePoints(int32 InPoints)
{
	AttributePoints = InPoints;
	OnAttirbutePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetSpellPoints(int32 InPoints)
{
	SpellPoints = InPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddToAttributePoints(int32 InAttributePoints)
{
	AttributePoints += InAttributePoints;
	OnAttirbutePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 InSpellPoints)
{
	SpellPoints += InSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	// ͨ���Ƚ��¾�ֵ���ж��Ƿ�������(������AIд�ģ��ж��Ƿ�����Ϊ��ɫ�������µĵȼ��仯��
	bool bIsLevelUp = (Level > OldLevel);
	OnLevelChangedDelegate.Broadcast(Level, bIsLevelUp);//ʹ�� true/false ���������������ʹ浵���������ֲ�ͬ�ĵȼ��仯�����
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttirbutePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}



