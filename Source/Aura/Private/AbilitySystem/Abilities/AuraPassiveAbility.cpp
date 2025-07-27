// Copyright Ivy


#include "AbilitySystem/Abilities/AuraPassiveAbility.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemBlueprintLibrary.h>

void UAuraPassiveAbility::ActivateAbility
(
	const FGameplayAbilitySpecHandle Handle,                // ����ʵ����Ψһ��ʶ
	const FGameplayAbilityActorInfo* ActorInfo,             // ӵ�иü��ܵ�Actor��Ϣ
	const FGameplayAbilityActivationInfo ActivationInfo,    // ������Ϣ
	const FGameplayEventData* TriggerEventData              // �����¼�����(�����)
)
{
	// ���ø����ActivateAbility�������л����������
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// ���Ի�ȡAvatarActor������ϵͳ���(ASC)��ת��ΪUAuraAbilitySystemComponent����
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		// ��ReceivedDeactivate�����󶨵�ASC��DeactivatePassiveAbilityί����
		// ��ASC��Ҫͣ�ñ�������ʱ����������ί��
		AuraASC->DeactivatePassiveAbility.AddUObject(this, &UAuraPassiveAbility::ReceivedDeactivate);
	}

}
//����˵��
//ActivateAbility :

//���������ܼ���ʱ����
//��Ҫ�����ǻ�ȡ��ɫ������ϵͳ�������ͣ��ί��
//����Ҫͣ�ñ�������ʱ������ϵͳ�����ͨ�����ί��֪ͨ����

//ReceivedDeactivate :

//�����յ�ͣ������ʱ����
//�������ͣ�õļ��ܱ�ǩ�Ƿ�ƥ�䵱ǰ����
//���ƥ�䣬�������ǰ����
// 
// 
//��δ�����Ҫ����ʵ�ֱ������ܵĻ�����ܣ����������ܱ��ⲿϵͳ(������ϵͳ���)ͨ����ǩƥ����ͣ�á�

void UAuraPassiveAbility::ReceivedDeactivate(const FGameplayTag& AbilityTag)// ����ͣ�ñ�����������
{
	// ��鴫��ļ��ܱ�ǩ�Ƿ��뵱ǰ���ܵı�ǩ��ȫƥ��
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		// ���ƥ�䣬�������ǰ����
			// ����˵��:
			// - CurrentSpecHandle: ��ǰ����ʵ���ľ��
			// - CurrentActorInfo: ��ǰActor��Ϣ
			// - CurrentActivationInfo: ��ǰ������Ϣ
			// - true: ��ʾ�Ƿ񴥷�����
			// - true: ��ʾ�����Ƿ�ɹ�����
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
