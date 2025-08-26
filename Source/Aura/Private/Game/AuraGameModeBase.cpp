// Copyright Ivy


#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerStart.h>
#include "Game/AuraGameInstance.h"

//保存数据到磁盘
void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)//LoadSlot：MVVM模式的视图模型，包含UI显示所需的数据   SlotIndex：多存档系统中的槽位索引，用于支持多个存档文件
{
	DeleteSlot(LoadSlot->LoadSlotName, SlotIndex);

	// 创建指定类型的SaveGame对象实例
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);

	// 将基类指针转换为具体的存档游戏类型
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);

	// 将加载槽位中的玩家名称赋值给存档数据
	LoadScreenSaveGame->PlayerName = LoadSlot->PlayerName;

	//将存档数据中SaveSlotStatus设为Taken
	LoadScreenSaveGame->SaveSlotStatus = Taken;

	// 将加载槽位中的地图名称赋值给存档数据
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();

	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

	// 将存档数据保存到指定槽位名称和索引的位置
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}
//两个函数的区别
//	         SaveSlotData	      GetSaveSlotData
//目的	     保存数据到磁盘	      读取数据从磁盘
//操作方向	 内存 → 磁盘	          磁盘 → 内存
//数据流向	 UI数据 → 存档文件	  存档文件 → 内存对象
//返回值	     void（无返回值）      ULoadScreenSaveGame*
//使用场景	 玩家点击保存时	      需要加载存档数据时


ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const//读取数据从磁盘("请把X号槽位的存档给我")
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
//GetSaveSlotData(LoadSlotName, SlotIndex)
//需要明确指定要加载哪个存档槽位
//参数：存档名称 + 槽位索引
//用途：加载任意指定的存档

//RetriveInGameSaveData()                 
//自动获取当前游戏的存档信息
//无参数：从GameInstance自动获取
//用途：加载当前正在玩的存档


	   /*两个函数的核心区别：
	   *
					GetSaveSlotData(管理员)	           RetriveInGameSaveData(助理)
		需要的信息	   书名 + 书架号	                      什么都不需要
		谁提供信息	   你必须提供	                      自动从GameInstance获取
		使用场景	       读取任意存档	                      读取当前游戏的存档*/

ULoadScreenSaveGame* AAuraGameModeBase::RetriveInGameSaveData()//读取数据("请把我现在正在玩的存档给我")
{
	//RetriveInGameSaveData 是 GetSaveSlotData 的便捷包装，专门用于获取当前游戏的存档数据！
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;

	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);

}

//这个函数专门用于保存"当前正在进行的游戏"的进度数据(SaveInGameProgressData)
void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject)
{
	// 1. 获取游戏实例（知道当前游戏会话的信息）
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	
	// 2. 从游戏实例获取当前游戏的存档位置信息
	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;// 当前存档的名称
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex; // 当前存档的索引
	// 3. 更新游戏实例中的玩家起始位置标签
	AuraGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;// 同步重生点信息 ，这里SaveObject->PlayerStartTag已经在AAuraCharacter::SaveProgress里面更新了

	// 4. 将存档数据保存到当前游戏的槽位
	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	// 检查指定槽位名称和索引的存档是否已存在
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		// 如果存档已存在，先删除旧存档以便创建新存档
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}


void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->LoadSlotName;
	const int32 SlotIndex = Slot->SlotIndex;

	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	TArray<AActor*>Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if (Actors.Num() > 0)
	{
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}
