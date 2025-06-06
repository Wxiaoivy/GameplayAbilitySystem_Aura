// Copyright Ivy


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP)const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		if (LevelUpInformation.Num() - 1 <= Level) return Level;
		
		if (LevelUpInformation[Level].LevelUpRequirement <= XP)
		{
			++Level;
		} 
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
