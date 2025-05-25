// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

/**
 * AuraGameplayTags
 * 
 * Singleton containing native Gameplay Tags(�����࣬����ԭ��Gameplay��ǩ)
 */

struct FAuraGameplayTags
{
public:
	// ��ȡ����ʵ���ľ�̬����
	static const FAuraGameplayTags& Get() {return GameplayTags;}
	// ��ʼ��ԭ��Gameplay��ǩ�ľ�̬����
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
	// ������̬��Ա�������洢����ʵ��
	// ����������߱�����������������ʵ������һ�� GameplayTags ������
	//�ص㣺// ��̬��Ա�������༶����ڣ���������ʵ������(��̬��Ա���������಻����ĳ��ʵ������
	       // �����ڴ���ֻ��һ�ݣ����۴������ٸ����ʵ������������һ��������
	
	static FAuraGameplayTags GameplayTags;

};