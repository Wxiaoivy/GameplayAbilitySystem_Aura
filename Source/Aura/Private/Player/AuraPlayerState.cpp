// Copyright Ivy


#include "Player/AuraPlayerState.h"

AAuraPlayerState::AAuraPlayerState()
{
	//是一个与网络更新或数据同步相关的频率设置
	//用于客户端和服务器之间的数据同步
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
	OnLevelChangedDelegate.Broadcast(Level,true); // 使用 true / false 来区分正常升级和存档加载这两种不同的等级变化情况。 这个AddToLevel函数只用于正常升级情况调用
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level,false);//使用 true/false 来区分正常升级和存档加载这两种不同的等级变化情况。 这个SetLevel函数只用于存档加载情况才调用
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
	// 通过比较新旧值来判断是否是升级(这里是AI写的，判断是否是因为角色升级导致的等级变化）
	bool bIsLevelUp = (Level > OldLevel);
	OnLevelChangedDelegate.Broadcast(Level, bIsLevelUp);//使用 true/false 来区分正常升级和存档加载这两种不同的等级变化情况。
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



