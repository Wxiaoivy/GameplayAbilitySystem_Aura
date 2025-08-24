// Copyright Ivy


#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)//LoadSlot：MVVM模式的视图模型，包含UI显示所需的数据   SlotIndex：多存档系统中的槽位索引，用于支持多个存档文件
{
	// 检查指定槽位名称和索引的存档是否已存在
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->LoadSlotName,SlotIndex))
	{
		// 如果存档已存在，先删除旧存档以便创建新存档
		UGameplayStatics::DeleteGameInSlot(LoadSlot->LoadSlotName, SlotIndex);
	}
	// 创建指定类型的SaveGame对象实例
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);

	// 将基类指针转换为具体的存档游戏类型
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);

	// 将加载槽位中的玩家名称赋值给存档数据
	LoadScreenSaveGame->PlayerName = LoadSlot->PlayerName;

	//将存档数据中SaveSlotStatus设为Taken
	LoadScreenSaveGame->SaveSlotStatus = Taken;

	// 将存档数据保存到指定槽位名称和索引的位置
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	// 声明一个SaveGame指针，初始化为空
	USaveGame* SaveGameProject = nullptr;
	// 检查指定槽位是否存在存档文件
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		// 如果存档存在，从指定槽位加载存档
		SaveGameProject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}

	else
	{
		// 如果存档不存在，创建一个新的存档对象
		SaveGameProject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}

	// 将通用的USaveGame指针转换为特定的ULoadScreenSaveGame类型
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameProject);
	// 返回转换后的存档对象（如果转换失败则返回nullptr）
	return LoadScreenSaveGame;
}
