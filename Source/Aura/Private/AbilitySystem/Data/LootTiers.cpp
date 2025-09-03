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
				/*Item �����ѭ����ѭ��������������� loot tier ���ñ��е�ģ��������ֱ����� Item������ӵ�������ģ�屾�������ǻ��ڸ�ģ�崴������ʵ����
				�Ӵ��뿴��FLootItem �ṹ����ܰ��������ֶΣ�����ǰ����ֻ��ʽ��������������
				��� FLootItem ����������Ҫ��ʼ�����ֶΣ�����ΨһID�����ɵȼ����������Եȣ���ֱ��ʹ�� Item �ᵼ����Щ�ֶΰ�������ģ���ֵ��������Ϊ��ʵ�������ֵ��*/
			}
		}
	}
	return ReturnItems;
}
