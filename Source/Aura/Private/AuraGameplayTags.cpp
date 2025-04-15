// Copyright Ivy


#include "AuraGameplayTags.h"

// 定义静态成员变量GameplayTags
//在类的源文件（.cpp 文件）中，你需要定义这个静态成员变量，
// 这就是 FAuraGameplayTags FAuraGameplayTags::GameplayTags; 这行代码的作用。
// 它告诉编译器，为 GameplayTags 分配内存。
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

// 初始化原生Gameplay标签的方法实现
void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	// 向Gameplay标签管理器中添加一个原生标签
	// FName("Attribute.Secondary.Armor") 是标签的唯一标识符
	// FString("Reduces damage taken,improves Block Chance") 是标签的描述

	/*
	*Primary Attribute
	*
	*/
	GameplayTags.Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Primary.Strength"), FString("Increase pyhsical damage"));
	GameplayTags.Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Primary.Intelligence"), FString("Increase magical damage"));
	GameplayTags.Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Primary.Resilience"), FString("Increase Armor and Armor Penetration"));
	GameplayTags.Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Primary.Vigor"), FString("Increase Health"));
	
	
	
	/*
	*Secondary Attribute
	*
	*/
	GameplayTags.Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.Armor"), FString("Reduces damage taken,improves Block Chance"));
	GameplayTags.Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.ArmorPenetration"), FString("Ignores percentage of enemy Armor, increases Critical Hit Chance"));
	GameplayTags.Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.BlockChance"), FString("Chance to cut incoming damage in half"));
	GameplayTags.Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.CriticalHitChance"), FString("Chance to double damage plus critical hit bonus"));
	GameplayTags.Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.CriticalHitDamage"), FString("Bonus damage added when a critical hit is scored"));
	GameplayTags.Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.CriticalHitResistance"), FString("Reduces critical hit chance of attacking enemies"));
	GameplayTags.Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.HealthRegeneration"), FString("Amount of Health regenerated every 1 second"));
	GameplayTags.Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.ManaRegeneration"), FString("Amount of Mana regenerated every 1 second"));
	GameplayTags.Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.MaxHealth"), FString("Maximum amount of Health obtainable"));
	GameplayTags.Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.MaxMana"), FString("Maximum amount of Mana obtainable"));


	/*
	*Input Tag
	*
	*/
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag_LMB"), FString("Input Tag For Left Mouse Button"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag_RMB"), FString("Input Tag For Right Mouse Button"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag_1"), FString("Input Tag For 1 key"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag_2"), FString("Input Tag For 2 key"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag_3"), FString("Input Tag For 3 key"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag_4"), FString("Input Tag For 4 key"));


	/*
	*Damage Types 
	*
	*/
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"), FString("Damage"));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage_Fire"), FString("Fire Damage Type"));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage_Lightning"), FString("Lightning Damage Type"));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage_Arcane"), FString("Arcane Damage Type"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage_Physical"), FString("Physical Damage Type"));


	/*
	*Resistance
	*
	*/
	GameplayTags.Attribute_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Resistance.Fire"), FString("Resistance to Fire Damage"));
	GameplayTags.Attribute_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Resistance.Lightning"), FString("Resistance to Lightning Damage"));
	GameplayTags.Attribute_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Resistance.Arcane"), FString("Resistance to Arcane Damage"));
	GameplayTags.Attribute_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Resistance.Physical"), FString("Resistance to Physical Damage"));


	/*
	* TMap of Damage Types to Resistance
	*
	*/
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Fire, GameplayTags.Attribute_Resistance_Fire);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Lightning, GameplayTags.Attribute_Resistance_Lightning);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Arcane, GameplayTags.Attribute_Resistance_Arcane);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Physical, GameplayTags.Attribute_Resistance_Physical);



	/*
	*Effect Tag
	*
	*/
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HitReact"), FString("Tag granted when hit reacting"));
}
