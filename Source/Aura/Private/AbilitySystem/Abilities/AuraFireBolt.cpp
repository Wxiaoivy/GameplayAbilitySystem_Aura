// Copyright Ivy


#include "AbilitySystem/Abilities/AuraFireBolt.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = DamageTypesMap[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT
		(

			//Title
			"<Title>FIRE BOLT</>\n\n"

			//Level
			"<Small>Level: </><Level>%d</>\n"
			//ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n"

			"<Default>Launches a bolt of fire,"
			"Exploding on impact and dealing: </>"

			//Damage
			"<Damage>%d</><Default>fire damage with a chance to burn</>"

		),
			//Values
			Level,
			ManaCost,
			Cooldown,
			Damage
		);
	}
	return FString::Printf(TEXT
	(

		//Title
		"<Title>FIRE BOLT</>\n\n"

		//Level
		"<Small>Level: </><Level>%d</>\n"
		//ManaCost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		//Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		//Firebolt Num
		"<Default>Launches %d bolt of fire,"
		"Exploding on impact and dealing: </>"

		//Damage
		"<Damage>%d</><Default>fire damage with a chance to burn</>"

	),
		//Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level,SpellNum),
		Damage
	);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = DamageTypesMap[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT
	(

		//Title
		"<Title>NEXT LEVEL</>\n\n"

		//Level
		"<Small>Level: </><Level>%d</>\n"
		//ManaCost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		//Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		//Firebolt Num
		"<Default>Launches %d bolt of fire,"
		"Exploding on impact and dealing: </>"

		//Damage
		"<Damage>%d</><Default>fire damage with a chance to burn</>"

	),
		//Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, SpellNum),
		Damage
	);
}


