// Copyright Ivy


#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Game/AuraGameInstance.h"

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
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);// 以索引0为键添加到映射

	// 创建第二个槽位的视图模型实例	
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");// 设置该槽位的存档文件名称为"LoadSlot_1"
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1, LoadSlot_1);// 以索引1为键添加到映射

	// 创建第三个槽位的视图模型实例
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");// 设置该槽位的存档文件名称为"LoadSlot_2"
	LoadSlot_2->SlotIndex = 2;
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
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	// 更新视图模型中的地图名称
	LoadSlots[Slot]->SetMapName(AuraGameMode->DefaultMapName);
	// 更新视图模型中的玩家名称
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	// 更新视图模型中的槽位状态
	LoadSlots[Slot]->SlotStatus = Taken;
	// 更新视图模型中的PlayerStartTag
	LoadSlots[Slot]->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;


	// 调用游戏模式的保存功能，将数据持久化到磁盘(这是自定义的函数)
	AuraGameMode->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();


	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	AuraGameInstance->LoadSlotName = LoadSlots[Slot]->LoadSlotName;
	AuraGameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
	AuraGameInstance->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;

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

/**
 * 当SelecteSlotButton按钮被按下时的处理函数
 * 参数： Slot 被选择的存档槽索引
 */
void UMVVM_LoadScreen::SelecteSlotButtonPressed(int32 Slot)
{
	// 广播槽位选择事件，通知其他系统有槽位被选择
	SlotSelected.Broadcast();
	// 遍历所有存档槽
	for (const TTuple<int32, UMVVM_LoadSlot*>LoadSlot : LoadSlots)
	{
		// 当前遍历到的槽位是被选择的槽位，禁用其选择按钮
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}
		else
		{
			// 其他槽位不是被选择的，启用它们的选择按钮
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
	// 更新当前选中的槽位引用
	SelectedSlot = LoadSlots[Slot];
}

/**
 * 删除按钮按下时的处理函数
 * 删除当前选中的存档槽数据
 */
void UMVVM_LoadScreen::DeleteButtonPressed()
{
	// 检查是否有有效的选中槽位
	if (IsValid(SelectedSlot))
	{
		// 调用游戏模式的静态方法删除存档文件(自定义的函数）
		AAuraGameModeBase::DeleteSlot(SelectedSlot->LoadSlotName, SelectedSlot->SlotIndex);
		// 更新槽位状态为空闲
		SelectedSlot->SlotStatus = Vacant;
		// 重新初始化槽位（重置显示信息等）
		SelectedSlot->InitializeSlot();
		// 启用该槽位的SelectSlot（因为已删除，可以重新选择)
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	AuraGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;//如果直接打开Dungeon关卡  不经过这个PlayButtonPressed函数，不按这个Play按钮， PlayerStartTag 就不会被正确初始化 就找不到正确的PlayerStart。
	AuraGameInstance->LoadSlotName = SelectedSlot->LoadSlotName;
	AuraGameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;

	/*? 通过Play按钮进入：调用 PlayButtonPressed() → 正确设置 PlayerStartTag → 找到正确PlayerStart
	? 直接打开关卡：跳过 PlayButtonPressed() → PlayerStartTag 未被设置 → 找不到匹配的PlayerStart*/

	if (IsValid(SelectedSlot))
	{
		AuraGameMode->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::LoadData()//数据流向：磁盘存档 → SaveObject → MVVM视图模型 → UI显示
{
	// 获取游戏模式基类实例
	// 通过UGameplayStatics获取当前游戏模式，并转换为AAuraGameModeBase类型
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	// 遍历所有加载槽位
		// LoadSlots 是一个 TMap<int32, UMVVM_LoadSlot*> 类型的容器
		// 包含槽位索引和对应的MVVM视图模型对象
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		// 从游戏模式获取指定槽位的存档数据
		// LoadSlot.Value->LoadSlotName: 从MVVM视图模型获取槽位名称
		// LoadSlot.Key: 槽位的索引键值
		ULoadScreenSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(LoadSlot.Value->LoadSlotName, LoadSlot.Key);

		// 从存档对象中读取玩家名称
		const FString PlayerName = SaveObject->PlayerName;

		// 从存档对象中读取槽位状态
        // ESaveSlotStatus 是一个枚举类型，表示槽位的状态（Vacant、EnterName、Taken）
		TEnumAsByte<ESaveSlotStatus>SaveSlotStatus = SaveObject->SaveSlotStatus;

		// 将读取到的数据设置到MVVM视图模型中
		LoadSlot.Value->SlotStatus = SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(PlayerName);
		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;


		// 初始化槽位，可能包括更新UI显示、绑定委托等操作
		LoadSlot.Value->InitializeSlot();

	}
}

