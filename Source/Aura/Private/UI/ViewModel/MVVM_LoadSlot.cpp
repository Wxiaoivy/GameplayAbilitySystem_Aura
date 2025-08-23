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
	SetWidgetSwitcherIndex.Broadcast(2);
}

