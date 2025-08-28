// Copyright Ivy


#include "UI/ViewModel/MVVM_LoadSlot.h"

/*AI�ӵ�  Ϊ����ȷʹ��MVVM*/
void UMVVM_LoadSlot::SetLoadSlotName(const FString& InLoadSlotName)
{
	if (LoadSlotName == InLoadSlotName)
		return;

	LoadSlotName = InLoadSlotName;

	// ʹ����ȷ������������
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::LoadSlotName);
}
/*AI�ӵ�  Ϊ����ȷʹ��MVVM*/

void UMVVM_LoadSlot::InitializeSlot()
{
	//TODO: Check slot status based on loaded data . 

	// ��ȡö��״̬������ֵ
	// SlotStatus ��һ�� TEnumAsByte<ESaveSlotStatus> ����
	// GetValue() ���صײ�ö�ٵ�����ֵ������UI�л�
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	// WidgetSwitcherIndex ��ӦUI�в�ͬ״̬����ʾ���磺Vacant��EnterName��Taken��
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

void UMVVM_LoadSlot::SetPlayerLevel(int32 InLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InLevel);

}
