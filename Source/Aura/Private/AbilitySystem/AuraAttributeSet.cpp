// Copyright Ivy


#include "AbilitySystem/AuraAttributeSet.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(150.f);
	InitMana(100.f);
	InitMaxMana(150.f);
}
// UAuraAttributeSet类的GetLifetimeReplicatedProps函数实现
// 这个函数会调用父类的同名函数，并添加Health属性到需要复制的属性列表中
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 使用DOREPLIFETIME_CONDITION_NOTIFY宏来指定Health属性需要复制，并且总是触发OnRep_Health函数
	// COND_None表示没有特定的条件需要满足，
	// REPNOTIFY_Always表示总是触发通知
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

// UAuraAttributeSet类的OnRep_Health函数实现
// 这个函数会在Health属性在网络上被复制时被调用
// GAMEPLAYATTRIBUTE_REPNOTIFY宏用于处理属性复制的通知逻辑
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//GAMEPLAYATTRIBUTE_REPNOTIFY这个宏专门用于处理游戏玩法属性（Gameplay Attributes）复制时的通知。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldHealth);
}
