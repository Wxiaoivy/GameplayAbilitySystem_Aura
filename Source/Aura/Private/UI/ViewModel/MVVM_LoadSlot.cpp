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
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}
