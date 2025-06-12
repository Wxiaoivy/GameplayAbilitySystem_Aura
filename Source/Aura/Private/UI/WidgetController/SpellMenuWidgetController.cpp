// Copyright Ivy


#include "UI/WidgetController/SpellMenuWidgetController.h"
#include <../../../../../../../Source/Runtime/GameplayTags/Classes/GameplayTagContainer.h>
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"



void USpellMenuWidgetController::BroadCastInitialValues()
{
	BroadcastAbilityInfo();// 调用父类方法广播所有能力信息
	OnSpellPointsChangedDelegateOnController.Broadcast(GetAuraPS()->GetSpellPoints());// 游戏刚开始时广播当前法术点数
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	//状态同步完整性：确保任何可能影响按钮状态的变化都会触发UI更新

	/*关注点分离：

		AbilityStatusChanged处理能力状态变化

		OnSpellPointsChanged处理资源变化

		一致性保证：无论变化来自哪个系统，UI都能保持正确状态

		最小化更新：只有当选中能力的状态或法术点数变化时才更新按钮*/


	// 绑定到能力系统组件的AbilityStatusChanged委托
	GetAuraASC()->AbilityStatusChanged.AddLambda
	([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 NewLevel)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))//检查当前变化的能力是否是我们选中的能力(SelectedAbility)
			{
				//如果是选中的能力，更新它的状态为新的状态  为了保证状态同步完整性：确保任何可能影响按钮状态的变化都会触发UI更新（选择法术球时，能力状态变化时，法术点变化时）
				SelectedAbility.Status = StatusTag;
				bool bEnableSpendPointsButton = false;// 初始化花费点数按钮状态
				bool bEnableEquipButton = false;// 初始化装备按钮状态
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// 调用辅助函数确定按钮状态
				FString Description;//声明两个字符串变量，用来存储当前等级描述和下一等级的描述
				FString NextLevelDescription;
				GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, Description, NextLevelDescription);//获取当前能力和下一等级的描述文本
				SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton,Description,NextLevelDescription); // 广播按钮状态和描述内容给UI

			}
			if (AbilityInfo)// 确保能力信息数据有效
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);// 获取指定标签的能力信息
				Info.StatusTag = StatusTag;// 更新为相匹配的状态标签
				AbilityInfoDelegate.Broadcast(Info);// 广播更新后的能力信息
			}
		}
	);
	// 绑定到玩家状态的OnSpellPointsChangedDelegate委托
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda
	(            [this](int32 SpellPoints)
		{
			OnSpellPointsChangedDelegateOnController.Broadcast(SpellPoints);// 当法术点数变化时，广播新值
			
			//作用：当法术点数变化时，重新评估按钮状态。为了保证状态同步完整性：确保任何可能影响按钮状态的变化都会触发UI更新（选择法术球时，能力状态变化时，法术点变化时）
			//即使能力状态没变，点数变化可能改变按钮可用性
			//例如：点数从0变为1，现在可以花费点数升级能力
			CurrentSpellPoints = SpellPoints;
			bool bEnableSpendPointsButton = false;// 初始化花费点数按钮状态
			bool bEnableEquipButton = false;// 初始化装备按钮状态
			ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// 调用辅助函数确定按钮状态
			FString Description;
			FString NextLevelDescription;
			GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton, Description, NextLevelDescription);// 广播按钮状态和描述内容给UI
		}
	);

	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)// 处理法术球被选中的逻辑
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();// 获取游戏标签单例
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints(); // 获取当前法术点数
	FGameplayTag StatusTag; // 用于存储能力状态

	const bool bTagValid = AbilityTag.IsValid();// 检查能力标签是否有效
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);// 检查是否是"无能力"标签
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);// 从能力系统获取能力规格
	const bool bSpecValid = AbilitySpec != nullptr; // 检查能力规格是否有效

	if (!bTagValid || bTagNone || !bSpecValid)// 如果标签无效、是无能力标签或规格无效
	{
		StatusTag = GameplayTags.Abilities_Status_Lock;// 设置为锁定状态
	}
	else
	{
		StatusTag = GetAuraASC()->GetStatusTagFormSpec(*AbilitySpec);// 否则从能力规格获取状态标签
	}
	
	SelectedAbility.Ability = AbilityTag;//每当选择法术球时都要重新设置该结构体,把更新后的值存到结构体里，
	SelectedAbility.Status = StatusTag;//为了保证状态同步完整性：确保任何可能影响按钮状态的变化都会触发UI更新（选择法术球时，能力状态变化时，法术点变化时）


	bool bEnableSpendPointsButton = false;// 初始化花费点数按钮状态
	bool bEnableEquipButton = false;// 初始化装备按钮状态
	ShouldEnableButtons(StatusTag, SpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// 调用辅助函数确定按钮状态
	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton, Description, NextLevelDescription); // 广播按钮状态和描述内容给UI

}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->SeverSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::GlobeDeselected()
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}
	

	SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Lock;

	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
	
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	WaitForEquipDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = true;

	FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection)return;
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType))return;
	
	GetAuraASC()->SeverEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(
	const FGameplayTag& AbilityTag,   // 被装备的技能（如 "Ability.Fire.Firebolt"）
	const FGameplayTag& Status,       // 技能的新状态（如 "Status.Equipped"）
	const FGameplayTag& Slot,         // 技能绑定的输入槽（如 "Input.LMB"）
	const FGameplayTag& PreviousSlot  // 之前绑定的槽位（可能为空）       //调用时机：当玩家在技能菜单中将某个技能拖拽到快捷键栏（如鼠标左键）时触发。
) 
{
	
	bWaitingForEquipSelection = false; // 标记“技能选择流程”结束（比如关闭技能选择弹窗）

	//关键步骤：清空旧绑定 → 2. 设置新绑定 → 3. 通知UI更新。
	//第一次广播 (LastSlotInfo)：清空旧槽位。
	//第二次广播(Info)：更新新槽位。
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;// 标记旧槽位为“可装备”(这个我不太能理解）
	LastSlotInfo.InputTag = PreviousSlot; // 记录旧槽位（如 "Input.LMB"）
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None; // 清空技能(在蓝图里把Abilities_None的法术球都设为透明，）
	AbilityInfoDelegate.Broadcast(LastSlotInfo);// 如果之前有技能绑定在 PreviousSlot（如鼠标左键原本绑定了火球术），现在需要 清空它。广播 LastSlotInfo 告诉UI：“这个槽位现在空了，可以显示默认图标”。

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);// 查找技能数据
	Info.StatusTag = Status;// 更新状态（如 "Equipped"）
	Info.InputTag = Slot; // 更新绑定的输入槽（如 "Input.LMB"）
	AbilityInfoDelegate.Broadcast(Info);// 通知UI更新  更新新装备的技能信息（如显示火球术图标到鼠标左键栏位）。广播 Info 告诉UI：“这个槽位现在绑定了新技能”。

	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType); //通知其他系统（如技能菜单UI）：“装备流程已完成”，可以关闭等待状态或刷新界面。

}

// 静态辅助函数，根据能力状态和法术点数决定按钮状态
void FORCENOINLINE USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get(); // 获取游戏标签
	bShouldEnableSpendPointsButton = false;// 初始化为false
	bShouldEnableEquipButton = false;
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) )// 如果是已装备状态
	{
		bShouldEnableEquipButton = true; // 装备按钮可用
		if (SpellPoints > 0)// 如果有法术点数
		{
			bShouldEnableSpendPointsButton = true;//花费点数按钮可用
		}
	} 
	else if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))// 如果是解锁状态
	{
		bShouldEnableEquipButton = true;// 装备按钮可用
		if (SpellPoints > 0)// 如果有法术点数
		{
			bShouldEnableSpendPointsButton = true;//花费点数按钮可用
		}
	}
	else if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))// 如果是符合条件状态
	{
		bShouldEnableEquipButton = false;// 装备按钮不可用
		if (SpellPoints > 0)// 如果有法术点数
		{
			bShouldEnableSpendPointsButton = true;//花费点数按钮可用
		}
	}
   //最后一个情况AbilityStatus == GameplayTags.Abilities_Status_Lock  两个布尔值都是false， 都是默认值了所以不用写出来了
}
