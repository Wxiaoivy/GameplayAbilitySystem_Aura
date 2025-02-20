// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemComponent.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent()
{

}

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//OnGameplayEffectAppliedDelegateToSelf�Ǹ����һ������ʵ��������󶨻ص��������˴���
	//OnGameplayEffectAppliedDelegateToSelfÿ��һ����ϷЧ����Game Effect�����GE����Ӧ�õ�������ʱ�������Ǽ�ʱЧ�����ǳ���Ч����������������ô˷�������
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{

	// ����һ����Ϸ�淨��ǩ������TagContainer�������ڴ洢��Ч���淶����ȡ�ı�ǩ��
	FGameplayTagContainer TagContainer;
	// ��Ч���淶�л�ȡ���е��ʲ���ǩ���������Ǵ洢��TagContainer�С�
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTag.Broadcast(TagContainer);//�㲥����WidgetController���������ݣ�
}

