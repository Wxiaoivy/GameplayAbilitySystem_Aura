// Copyright Ivy


#include "UI/ViewModel/MVVM_LoadSlot.h"


void UMVVM_LoadSlot::SetLoadSlotName(const FString& InLoadSlotName)
{
	if (LoadSlotName == InLoadSlotName)
		return;

	LoadSlotName = InLoadSlotName;

	// 使用正确的描述符名称
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::LoadSlotName);
}
