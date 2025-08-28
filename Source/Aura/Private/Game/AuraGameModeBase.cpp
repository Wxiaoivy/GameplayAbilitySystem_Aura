// Copyright Ivy


#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerStart.h>
#include "Game/AuraGameInstance.h"
#include <../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h>
#include "Interaction/SaveInterface.h"


void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	// ���ָ����λ���ƺ������Ĵ浵�Ƿ��Ѵ���
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		// ����浵�Ѵ��ڣ���ɾ���ɴ浵�Ա㴴���´浵
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

//�������ݵ�����
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
//��������������
//	         SaveSlotData	      GetSaveSlotData
//Ŀ��	     �������ݵ�����	      ��ȡ���ݴӴ���
//��������	 �ڴ� �� ����	          ���� �� �ڴ�
//��������	 UI���� �� �浵�ļ�	  �浵�ļ� �� �ڴ����
//����ֵ	     void���޷���ֵ��      ULoadScreenSaveGame*
//ʹ�ó���	 ��ҵ������ʱ	      ��Ҫ���ش浵����ʱ


ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const//��ȡ���ݴӴ���("���X�Ų�λ�Ĵ浵����")
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
//GetSaveSlotData(LoadSlotName, SlotIndex)
//��Ҫ��ȷָ��Ҫ�����ĸ��浵��λ
//�������浵���� + ��λ����
//��;����������ָ���Ĵ浵

//RetriveInGameSaveData()                 
//�Զ���ȡ��ǰ��Ϸ�Ĵ浵��Ϣ
//�޲�������GameInstance�Զ���ȡ
//��;�����ص�ǰ������Ĵ浵


	   /*���������ĺ�������
	   *
					GetSaveSlotData(����Ա)	           RetriveInGameSaveData(����)
		��Ҫ����Ϣ	   ���� + ��ܺ�	                      ʲô������Ҫ
		˭�ṩ��Ϣ	   ������ṩ	                      �Զ���GameInstance��ȡ
		ʹ�ó���	       ��ȡ����浵	                      ��ȡ��ǰ��Ϸ�Ĵ浵*/

ULoadScreenSaveGame* AAuraGameModeBase::RetriveInGameSaveData()//��ȡ����("���������������Ĵ浵����")
{
	//RetriveInGameSaveData �� GetSaveSlotData �ı�ݰ�װ��ר�����ڻ�ȡ��ǰ��Ϸ�Ĵ浵���ݣ�
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;

	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);

}

//�������ר�����ڱ���"��ǰ���ڽ��е���Ϸ"�Ľ�������(SaveInGameProgressData)
void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject)
{
	// 1. ��ȡ��Ϸʵ����֪����ǰ��Ϸ�Ự����Ϣ��
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	
	// 2. ����Ϸʵ����ȡ��ǰ��Ϸ�Ĵ浵λ����Ϣ
	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;// ��ǰ�浵������
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex; // ��ǰ�浵������
	// 3. ������Ϸʵ���е������ʼλ�ñ�ǩ
	AuraGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;// ͬ����������Ϣ ������SaveObject->PlayerStartTag�Ѿ���AAuraCharacter::SaveProgress���������

	// 4. ���浵���ݱ��浽��ǰ��Ϸ�Ĳ�λ
	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}



void AAuraGameModeBase::SaveWorldState(UWorld* World)
{
	// ��ȡ��ǰ��������ƣ����Ƴ���ʽ�ؿ�ǰ׺����"UEDPIE_0_"��
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	// ��ȡ��Ϸʵ����ת��Ϊ�Զ����UAuraGameInstance����
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);// ȷ����Ϸʵ����Ч

	// ��ȡָ����λ�Ĵ浵����
	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
	{
		// ���浵���Ƿ����е�ǰ��ͼ�ı������ݣ����û���򴴽��µ�
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}

		// ��ȡ��ǰ��ͼ�ı�������
		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty();// ������е���Ա����

		// ���������е�����Actor
		/*FActorIterator ��UE�ṩ��һ����Ч������������Actor�Ĺ��ߡ�����
			ֱ�ӷ���������ڲ�Actor�б�
			�������죺û����ʱ������俪��
			�����ã���׼��C++������ģʽ
			����ǿ�󣺿��Է��ʵ��������͵�Actor*/
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			// ��������Actorͨ����Ҫ����
			//���Actor��Ч����û��ʵ��SaveInterface������
			if (IsValid(Actor) || !Actor->Implements<USaveInterface>())continue;
		  
			// ���������Actor����
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			// �����ڴ�д�������������л�Actor���ݵ�Bytes����
			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			// �������л���־������Actorֻ������ΪSaveGame������
			MemoryWriter.ArIsSaveGame = true;

			// Actor���Լ����ΪSaveGame������д��MemoryWriter
            // MemoryWriter����Щ���ݱ��浽SavedActor.Bytes��
			Actor->Serialize(MemoryWriter);

			// �������Actor��ӵ���ͼ�ı����б���
			SavedMap.SavedActors.AddUnique(SavedActor);
		}

		// �����浵�е������ѱ����ͼ��������Ҫ���µĵ�ͼ��¼
		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				// ���ҵ��뵱ǰ��ͼ����ƥ��ļ�¼ʱ�����µ�SavedMap�����滻������
				MapToReplace = SavedMap;
				break;  // �ҵ����滻�������˳�ѭ�������Ч��
			}
			
		}
		// �����º�Ĵ浵���󱣴浽ָ���Ĵ��̲�λ(// ����˵���� - SaveGame: Ҫ����Ĵ浵���󣨰������и��º�����ݣ�)
		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
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
