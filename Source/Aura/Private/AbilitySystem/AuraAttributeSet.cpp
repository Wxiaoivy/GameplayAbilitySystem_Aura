// Copyright Ivy


#include "AbilitySystem/AuraAttributeSet.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(75.f);
	InitMaxMana(100.f);
}
// UAuraAttributeSet���GetLifetimeReplicatedProps����ʵ��
// �����������ø����ͬ�������������Health���Ե���Ҫ���Ƶ������б���
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ʹ��DOREPLIFETIME_CONDITION_NOTIFY����ָ��Health������Ҫ���ƣ��������Ǵ���OnRep_Health����
	// COND_None��ʾû���ض���������Ҫ���㣬
	// REPNOTIFY_Always��ʾ���Ǵ���֪ͨ
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
 
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

// UAuraAttributeSet���OnRep_Health����ʵ��
// �����������Health�����������ϱ�����ʱ������
// GAMEPLAYATTRIBUTE_REPNOTIFY�����ڴ������Ը��Ƶ�֪ͨ�߼�
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//GAMEPLAYATTRIBUTE_REPNOTIFY�����ר�����ڴ�����Ϸ�淨���ԣ�Gameplay Attributes������ʱ��֪ͨ��
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}
