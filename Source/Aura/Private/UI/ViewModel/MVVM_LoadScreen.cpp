// Copyright Ivy


#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

/*AI�ӵ�  Ϊ����ȷʹ��MVVM*/
UMVVM_LoadScreen::UMVVM_LoadScreen()//AI
{
	LoadScreenName = TEXT("Default Load Screen");//AI
}

void UMVVM_LoadScreen::SetLoadScreenName(const FString& NewName)//AI
{
	if (LoadScreenName == NewName)//AI
		return;//AI

	LoadScreenName = NewName;//AI

	// UE5.4 ����ȷ��ʽ - ʹ����򵥵Ĺ㲥����
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::LoadScreenName);//AI
}
/*AI�ӵ�  Ϊ����ȷʹ��MVVM*/







//�������̣�
//��ʼ�� �� ����3����λVM
//
//��ȡVM �� ͨ��������ȡ�ض���λ
//
//�������� �� �������� �� ����VM �� ������Ϸģʽ����
//
//��ʼ��Ϸ �� �����ź��л�UI״̬



/**
 * ��ʼ�����ز�λ��ͼģ��
 * �����������ز�λ��ViewModelʵ������ӵ�ӳ����
 */
void UMVVM_LoadScreen::InitializeLoadSlots()
{
	// ������һ����λ����ͼģ��ʵ��	
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");// ���øò�λ�Ĵ浵�ļ�����Ϊ"LoadSlot_0"����Ӧ�����ϵĴ浵�ļ�
	LoadSlots.Add(0, LoadSlot_0);// ������0Ϊ����ӵ�ӳ��

	// �����ڶ�����λ����ͼģ��ʵ��	
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");// ���øò�λ�Ĵ浵�ļ�����Ϊ"LoadSlot_1"
	LoadSlots.Add(1, LoadSlot_1);// ������1Ϊ����ӵ�ӳ��

	// ������������λ����ͼģ��ʵ��
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");// ���øò�λ�Ĵ浵�ļ�����Ϊ"LoadSlot_2"
	LoadSlots.Add(2, LoadSlot_2);// ������2Ϊ����ӵ�ӳ��
}

/**
 * ����������ȡ���ز�λ��ͼģ��
 *
 * @param Index ��λ������0, 1, 2��
 * @return ��Ӧ��UMVVM_LoadSlot��ͼģ��ָ��
 * @warning ������������ڻᴥ�����Ա�����ʹ��FindChecked��
 */
UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);// ��ȫ���գ�����������ڣ��������
}

/**
 * �²�λ��ť���´��� - ��������Ϸ����
 *
 * @param Slot ѡ��Ĳ�λ����
 * @param EnteredName ������������
 */
void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	// ��ȡ��Ϸģʽ����ʵ��
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	// ������ͼģ���е��������
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	// ������ͼģ���еĲ�λ״̬
	LoadSlots[Slot]->SlotStatus = Taken;
	// ������Ϸģʽ�ı��湦�ܣ������ݳ־û�������(�����Զ���ĺ���)
	AuraGameModeBase->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();
}

/**
 * ����Ϸ��ť���´��� - �л�����״̬
 *
 * @param Slot ѡ��Ĳ�λ����
 * �˺�������UI�л�����Ϸ��ʼ״̬
 */
void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	// �㲥ί�У�֪ͨUI�л�WidgetSwitcher��ҳ��������1
	// ͨ�����ڴӽ�ɫ����/ѡ������л�����Ϸ������
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelecteSlotButtonPressed(int32 Slot)
{

}

void UMVVM_LoadScreen::LoadData()//�������򣺴��̴浵 �� SaveObject �� MVVM��ͼģ�� �� UI��ʾ
{
	// ��ȡ��Ϸģʽ����ʵ��
	// ͨ��UGameplayStatics��ȡ��ǰ��Ϸģʽ����ת��ΪAAuraGameModeBase����
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	// �������м��ز�λ
		// LoadSlots ��һ�� TMap<int32, UMVVM_LoadSlot*> ���͵�����
		// ������λ�����Ͷ�Ӧ��MVVM��ͼģ�Ͷ���
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		// ����Ϸģʽ��ȡָ����λ�Ĵ浵����
		// LoadSlot.Value->LoadSlotName: ��MVVM��ͼģ�ͻ�ȡ��λ����
		// LoadSlot.Key: ��λ��������ֵ
		ULoadScreenSaveGame* SaveObject = AuraGameModeBase->GetSaveSlotData(LoadSlot.Value->LoadSlotName, LoadSlot.Key);

		// �Ӵ浵�����ж�ȡ�������
		const FString PlayerName = SaveObject->PlayerName;

		// �Ӵ浵�����ж�ȡ��λ״̬
        // ESaveSlotStatus ��һ��ö�����ͣ���ʾ��λ��״̬��Vacant��EnterName��Taken��
		TEnumAsByte<ESaveSlotStatus>SaveSlotStatus = SaveObject->SaveSlotStatus;

		// ����ȡ�����������õ�MVVM��ͼģ����
		LoadSlot.Value->SlotStatus = SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(PlayerName);

		// ��ʼ����λ�����ܰ�������UI��ʾ����ί�еȲ���
		LoadSlot.Value->InitializeSlot();
	}
}

