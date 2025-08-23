// Copyright Ivy


#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

/*AI加的  为了正确使用MVVM*/
UMVVM_LoadScreen::UMVVM_LoadScreen()//AI
{
	LoadScreenName = TEXT("Default Load Screen");//AI
}

void UMVVM_LoadScreen::SetLoadScreenName(const FString& NewName)//AI
{
	if (LoadScreenName == NewName)//AI
		return;//AI

	LoadScreenName = NewName;//AI

	// UE5.4 的正确方式 - 使用最简单的广播方法
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::LoadScreenName);//AI
}
/*AI加的  为了正确使用MVVM*/







//功能流程：
//初始化 → 创建3个槽位VM
//
//获取VM → 通过索引获取特定槽位
//
//保存数据 → 输入名称 → 更新VM → 调用游戏模式保存
//
//开始游戏 → 发送信号切换UI状态



/**
 * 初始化加载槽位视图模型
 * 创建三个加载槽位的ViewModel实例并添加到映射中
 */
void UMVVM_LoadScreen::InitializeLoadSlots()
{
	// 创建第一个槽位的视图模型实例	
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");// 设置该槽位的存档文件名称为"LoadSlot_0"，对应磁盘上的存档文件
	LoadSlots.Add(0, LoadSlot_0);// 以索引0为键添加到映射

	// 创建第二个槽位的视图模型实例	
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");// 设置该槽位的存档文件名称为"LoadSlot_1"
	LoadSlots.Add(1, LoadSlot_1);// 以索引1为键添加到映射

	// 创建第三个槽位的视图模型实例
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");// 设置该槽位的存档文件名称为"LoadSlot_2"
	LoadSlots.Add(2, LoadSlot_2);// 以索引2为键添加到映射
}

/**
 * 根据索引获取加载槽位视图模型
 *
 * @param Index 槽位索引（0, 1, 2）
 * @return 对应的UMVVM_LoadSlot视图模型指针
 * @warning 如果索引不存在会触发断言崩溃（使用FindChecked）
 */
UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);// 安全风险：索引必须存在，否则崩溃
}

/**
 * 新槽位按钮按下处理 - 保存新游戏数据
 *
 * @param Slot 选择的槽位索引
 * @param EnteredName 玩家输入的名称
 */
void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	// 获取游戏模式基类实例
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	// 更新视图模型中的玩家名称
	LoadSlots[Slot]->PlayerName = EnteredName;
	// 调用游戏模式的保存功能，将数据持久化到磁盘
	AuraGameModeBase->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();
}

/**
 * 新游戏按钮按下处理 - 切换界面状态
 *
 * @param Slot 选择的槽位索引
 * 此函数负责将UI切换到游戏开始状态
 */
void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	// 广播委托，通知UI切换WidgetSwitcher的页面索引到1
	// 通常用于从角色创建/选择界面切换到游戏主界面
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelecteSlotButtonPressed(int32 Slot)
{

}

