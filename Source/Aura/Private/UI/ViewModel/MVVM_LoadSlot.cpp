// Copyright Ivy


#include "UI/ViewModel/MVVM_LoadSlot.h"

/*AI加的  为了正确使用MVVM*/
void UMVVM_LoadSlot::SetLoadSlotName(const FString& InLoadSlotName)
{
	if (LoadSlotName == InLoadSlotName)
		return;

	LoadSlotName = InLoadSlotName;

	// 使用正确的描述符名称
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::LoadSlotName);
}
/*AI加的  为了正确使用MVVM*/

void UMVVM_LoadSlot::InitializeSlot()
{
	//TODO: Check slot status based on loaded data . 
	SetWidgetSwitcherIndex.Broadcast(2);
}

