// Copyright Ivy


#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerStart.h>
#include "Game/AuraGameInstance.h"

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
	UE_LOG(LogTemp, Warning, TEXT("ChoosePlayerStart called"));

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is null!"));
		return nullptr;
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	UE_LOG(LogTemp, Warning, TEXT("Found %d PlayerStarts"), Actors.Num());

	if (Actors.Num() > 0)
	{
		AActor* SelectedActor = Actors[0];
		UE_LOG(LogTemp, Warning, TEXT("Default: %s"), *SelectedActor->GetName());

		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				UE_LOG(LogTemp, Warning, TEXT("Checking: %s, Tag: %s"),
					*PlayerStart->GetName(), *PlayerStart->PlayerStartTag.ToString());

				if (!PlayerStart->PlayerStartTag.IsNone() && PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					UE_LOG(LogTemp, Warning, TEXT("MATCH FOUND: %s"), *PlayerStart->GetName());
					break;
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Returning: %s"), *SelectedActor->GetName());
		return SelectedActor; // 这里肯定返回了SelectedActor
	}

	UE_LOG(LogTemp, Error, TEXT("No PlayerStarts, returning nullptr"));
	return nullptr; // 只有这里才会返回nullptr
	//UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	//TArray<AActor*>Actors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	//if (Actors.Num() > 0)
	//{
	//	AActor* SelectedActor = Actors[0];
	//	for (AActor* Actor : Actors)
	//	{
	//		if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
	//		{
	//			if (PlayerStart->PlayerStartTag== AuraGameInstance->PlayerStartTag)
	//			{
	//				SelectedActor = PlayerStart;
	//				break;
	//			}
	//		}
	//	}
	//	return SelectedActor;
	//}
	//return nullptr;
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}
