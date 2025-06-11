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
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())//尝试获取这个技能的CostGameplayEffect(GameplayEffect)
	{
		for (FGameplayModifierInfo Mod:CostEffect->Modifiers)//遍历这个效果中的所有属性修改器(Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())//检查当前修改器是否作用于法力(Mana)属性
			{
				//尝试获取静态的法力消耗数值
				//GetStaticMagnitudeIfPossible是虚幻引擎提供的方法

				//	作用：如果这个修改器的数值是静态定义的(不是动态计算的)，就返回该数值
				//	参数：
				//	InLevel : 技能等级，用于可能的等级缩放
				//	ManaCost : 输出参数，接收计算后的法力消耗值
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
			    break;//找到法力属性后就跳出循环，不需要继续检查其他修改器
			}
		}
	}
	return ManaCost;//返回计算得到的法力消耗值
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
