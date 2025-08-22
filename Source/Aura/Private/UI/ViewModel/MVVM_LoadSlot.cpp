// Copyright Ivy


#include "UI/ViewModel/MVVM_LoadSlot.h"


void UMVVM_LoadSlot::SetLoadSlotName(const FString& InLoadSlotName)
{
	if (LoadSlotName == InLoadSlotName)
		return;

	LoadSlotName = InLoadSlotName;

	// ʹ����ȷ������������
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::LoadSlotName);
}
