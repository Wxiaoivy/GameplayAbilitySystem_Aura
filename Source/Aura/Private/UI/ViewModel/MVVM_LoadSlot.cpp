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

	// 获取枚举状态的整数值
	// SlotStatus 是一个 TEnumAsByte<ESaveSlotStatus> 类型
	// GetValue() 返回底层枚举的整数值，用于UI切换
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	// WidgetSwitcherIndex 对应UI中不同状态的显示（如：Vacant、EnterName、Taken）
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
