// Copyright Ivy


#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerStart.h>
#include "Game/AuraGameInstance.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)//LoadSlot��MVVMģʽ����ͼģ�ͣ�����UI��ʾ���������   SlotIndex����浵ϵͳ�еĲ�λ����������֧�ֶ���浵�ļ�
{
	DeleteSlot(LoadSlot->LoadSlotName, SlotIndex);

	// ����ָ�����͵�SaveGame����ʵ��
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);

	// ������ָ��ת��Ϊ����Ĵ浵��Ϸ����
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);

	// �����ز�λ�е�������Ƹ�ֵ���浵����
	LoadScreenSaveGame->PlayerName = LoadSlot->PlayerName;

	//���浵������SaveSlotStatus��ΪTaken
	LoadScreenSaveGame->SaveSlotStatus = Taken;

	// �����ز�λ�еĵ�ͼ���Ƹ�ֵ���浵����
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();

	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

	// ���浵���ݱ��浽ָ����λ���ƺ�������λ��
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	// ����һ��SaveGameָ�룬��ʼ��Ϊ��
	USaveGame* SaveGameProject = nullptr;
	// ���ָ����λ�Ƿ���ڴ浵�ļ�
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		// ����浵���ڣ���ָ����λ���ش浵
		SaveGameProject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}

	else
	{
		// ����浵�����ڣ�����һ���µĴ浵����
		SaveGameProject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}

	// ��ͨ�õ�USaveGameָ��ת��Ϊ�ض���ULoadScreenSaveGame����
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameProject);
	// ����ת����Ĵ浵�������ת��ʧ���򷵻�nullptr��
	return LoadScreenSaveGame;
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	// ���ָ����λ���ƺ������Ĵ浵�Ƿ��Ѵ���
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		// ����浵�Ѵ��ڣ���ɾ���ɴ浵�Ա㴴���´浵
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
		return SelectedActor; // ����϶�������SelectedActor
	}

	UE_LOG(LogTemp, Error, TEXT("No PlayerStarts, returning nullptr"));
	return nullptr; // ֻ������Ż᷵��nullptr
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
