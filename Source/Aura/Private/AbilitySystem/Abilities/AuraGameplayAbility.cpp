// Copyright Ivy


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString();
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString();
}

FString UAuraGameplayAbility::GetLockedDescription(int32 RequirementLevel)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level %d</>"), RequirementLevel);
}

float UAuraGameplayAbility::GetManaCost(float InLevel /*= 1.f*/)
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())//���Ի�ȡ������ܵ�CostGameplayEffect(GameplayEffect)
	{
		for (FGameplayModifierInfo Mod:CostEffect->Modifiers)//�������Ч���е����������޸���(Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())//��鵱ǰ�޸����Ƿ������ڷ���(Mana)����
			{
				//���Ի�ȡ��̬�ķ���������ֵ
				//GetStaticMagnitudeIfPossible����������ṩ�ķ���

				//	���ã��������޸�������ֵ�Ǿ�̬�����(���Ƕ�̬�����)���ͷ��ظ���ֵ
				//	������
				//	InLevel : ���ܵȼ������ڿ��ܵĵȼ�����
				//	ManaCost : ������������ռ����ķ�������ֵ
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
			    break;//�ҵ��������Ժ������ѭ��������Ҫ������������޸���
			}
		}
	}
	return ManaCost;//���ؼ���õ��ķ�������ֵ
}

float UAuraGameplayAbility::GetCooldown(float InLevel /*= 1.f*/)
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}
