// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

/**
 * AuraGameplayTags
 * 
 * Singleton containing native Gameplay Tags(单例类，包含原生Gameplay标签)
 */

struct FAuraGameplayTags
{
public:
	// 获取单例实例的静态方法
	static const FAuraGameplayTags& Get() {return GameplayTags;}
	// 初始化原生Gameplay标签的静态方法
	static void InitializeNativeGameplayTags();
	/*
	*Primary Attribute
	*
	*/
	FGameplayTag Attribute_Primary_Strength;
	FGameplayTag Attribute_Primary_Intelligence;
	FGameplayTag Attribute_Primary_Resilience;
	FGameplayTag Attribute_Primary_Vigor;


	/*
	*Secondary Attribute
	*
	*/
	FGameplayTag Attribute_Secondary_Armor;
	FGameplayTag Attribute_Secondary_ArmorPenetration;
	FGameplayTag Attribute_Secondary_BlockChance;
	FGameplayTag Attribute_Secondary_CriticalHitChance;
	FGameplayTag Attribute_Secondary_CriticalHitDamage;
	FGameplayTag Attribute_Secondary_CriticalHitResistance;
	FGameplayTag Attribute_Secondary_HealthRegeneration;
	FGameplayTag Attribute_Secondary_ManaRegeneration;
	FGameplayTag Attribute_Secondary_MaxHealth;
	FGameplayTag Attribute_Secondary_MaxMana;

	FGameplayTag Attribute_Resistance_Fire;
	FGameplayTag Attribute_Resistance_Lightning;
	FGameplayTag Attribute_Resistance_Arcane;
	FGameplayTag Attribute_Resistance_Physical;


	/*
	*Meta Attribute
	*
	*/
	FGameplayTag Attribute_Meta_IncomingXP;

	/*
	*Input Tag
	*
	*/

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;


	/*
	*Damage Tag
	*
	*/
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;
	

	TMap<FGameplayTag, FGameplayTag>DamageTypesToResistance;



	/*
	*Effect Tag
	*
	*/
	FGameplayTag Effects_HitReact;



	/*
	*Abilities Tag
	*
	*/
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;
	FGameplayTag Abilities_Fire_FireBolt;

	/*
	*Cooldown Tag
	*
	*/

	FGameplayTag Cooldown_Fire_FireBolt;



	/*
	*CombatSocket Tag
	*
	*/
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;

	/*
	*Montage Tag
	*
	*/
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

private:
	// 声明静态成员变量，存储单例实例
	// 这个声明告诉编译器，这个类的所有实例共享一个 GameplayTags 变量。
	//特点：// 静态成员变量在类级别存在，而不是在实例级别(静态成员变量属于类不属于某个实例）。
	       // 它在内存中只有一份，无论创建多少个类的实例，都共享这一个变量。
	
	static FAuraGameplayTags GameplayTags;

};