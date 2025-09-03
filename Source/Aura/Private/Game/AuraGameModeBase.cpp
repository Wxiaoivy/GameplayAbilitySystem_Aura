// Copyright Ivy


#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerStart.h>
#include "Game/AuraGameInstance.h"
#include <../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h>
#include "Interaction/SaveInterface.h"
#include <../../../../../../../Source/Runtime/CoreUObject/Public/Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/Character.h>


void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	// 检查指定槽位名称和索引的存档是否已存在
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		// 如果存档已存在，先删除旧存档以便创建新存档
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

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

	LoadScreenSaveGame->MapAssetName=LoadSlot->MapAssetName;

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



void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName)const
{
	// 获取当前世界的名称，并移除流式关卡前缀（如"UEDPIE_0_"）
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	// 获取游戏实例并转换为自定义的UAuraGameInstance类型
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);// 确保游戏实例有效

	// 获取指定槽位的存档数据
	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
	{
		if (DestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
		}
		// 检查存档中是否已有当前地图的保存数据，如果没有则创建新的
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}

		// 获取当前地图的保存数据
		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty();// 清空已有的演员数据

		// 遍历世界中的所有Actor
		/*FActorIterator 是UE提供的一个高效遍历世界所有Actor的工具。它：
			直接访问世界的内部Actor列表
			性能优异：没有临时数组分配开销
			简单易用：标准的C++迭代器模式
			功能强大：可以访问到所有类型的Actor*/
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			// 遍历所有Actor通常需要过滤
			//如果Actor无效或者没有实现SaveInterface就跳过
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())continue;
		  
			// 创建保存的Actor数据
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			// 创建内存写入器，用于序列化Actor数据到Bytes数组
			FMemoryWriter MemoryWriter(SavedActor.Bytes);

			// 创建代理存档器，它能够将对象和名称以字符串形式序列化
            // 第二个参数true表示使用引用方式（节省内存，避免重复数据）
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);

			// 设置序列化标志，告诉Actor只保存标记为SaveGame的属性
			Archive.ArIsSaveGame = true;

			// Actor将自己标记为SaveGame的属性写入MemoryWriter
            // MemoryWriter将这些数据保存到SavedActor.Bytes中
			Actor->Serialize(Archive);

			// 将保存的Actor添加到地图的保存列表中
			// AddUnique使用之前定义的operator==来避免重复保存同一个Actor
			SavedMap.SavedActors.AddUnique(SavedActor);
		}

		// 遍历存档中的所有已保存地图，查找需要更新的地图记录
		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				// 当找到与当前地图名称匹配的记录时，用新的SavedMap数据替换旧数据
				MapToReplace = SavedMap;
				break;  // 找到并替换后立即退出循环，提高效率
			}
			
		}
		// 将更新后的存档对象保存到指定的磁盘槽位(// 参数说明： - SaveGame: 要保存的存档对象（包含所有更新后的数据）)
		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
	}
}


//作用：从存档中加载世界状态，包括所有实现了SaveInterface的Actor的变换信息和自定义属性。
void AAuraGameModeBase::LoadWorldState(UWorld* World)const
{
	// 获取当前世界的名称，并移除流式关卡前缀（如"UEDPIE_0_"）
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);// 确保游戏实例有效

	// 检查指定槽位的存档是否存在
	if (UGameplayStatics::DoesSaveGameExist(AuraGI->LoadSlotName,AuraGI->LoadSlotIndex))
	{
		// 从槽位加载存档游戏
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex));
		if (SaveGame==nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load slot"));
			return;
		}
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			// 遍历所有Actor通常需要过滤
			//如果Actor无效或者没有实现SaveInterface就跳过
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())continue;

			// 遍历存档中保存的该地图的所有Actor数据
			for (FSavedActor SavedActor:SaveGame->GetSavedMapWithMapName(WorldName).SavedActors)
			{
				// 找到与当前Actor名称匹配的存档数据
				if (SavedActor.ActorName==Actor ->GetFName())
				{
					// 如果Actor需要加载变换信息，则设置其位置、旋转、缩放
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}

					// 使用内存读取器读取保存的字节数据
					FMemoryReader MemoryReader(SavedActor.Bytes);

					// 创建代理存档用于序列化
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;

					// 将保存的数据反序列化到Actor中（恢复属性值）
					Actor->Serialize(Archive);

					// 调用Actor的加载后处理函数
					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->LoadSlotName;
	const int32 SlotIndex = Slot->SlotIndex;

	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName)const
{
	for (auto& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

void AAuraGameModeBase::PlayerDied(ACharacter* DeadCharacter)
{
	// 从存储系统中检索当前的游戏存档数据
	ULoadScreenSaveGame* SaveGame = RetriveInGameSaveData();
	// 如果存档无效，直接返回（防止空指针操作）
	if (!IsValid(SaveGame))return;

	// 使用存档中记录的地图资源名称，重新加载对应关卡
    // 实现玩家死亡后的关卡重置或重生逻辑
	// 这里必须使用 MapAssetName（完整资源路径）
	// 因为引擎需要确切知道哪个资源文件要加载
	UGameplayStatics::OpenLevel(DeadCharacter, FName(SaveGame->MapAssetName));

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
