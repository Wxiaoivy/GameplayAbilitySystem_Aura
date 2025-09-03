// Copyright Ivy


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems()
{
	TArray<FLootItem>ReturnItems;

	for (const FLootItem& Item : LootItems)
	{
		for (int32 i = 0; i < Item.MaxNumberToSpawn; ++i)
		{
			if (FMath::RandRange(0.f, 100.f) < Item.ChanceToSpawn)
			{
				FLootItem NewItem;
				NewItem.LootClass = Item.LootClass;
				NewItem.bLootLevelOverride = Item.bLootLevelOverride;
				ReturnItems.Add(NewItem);
				/*Item 是外层循环的循环变量，代表的是 loot tier 配置表中的模板项。如果你直接添加 Item，你添加的是配置模板本身，而不是基于该模板创建的新实例。
				从代码看，FLootItem 结构体可能包含更多字段，而当前代码只显式设置了其中两个
				如果 FLootItem 还有其他需要初始化的字段（比如唯一ID、生成等级、附加属性等），直接使用 Item 会导致这些字段包含的是模板的值，而不是为新实例计算的值。*/
			}
		}
	}
	return ReturnItems;
}
