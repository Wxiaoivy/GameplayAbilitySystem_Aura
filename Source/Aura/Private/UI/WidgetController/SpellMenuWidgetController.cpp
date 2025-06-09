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

	// 绑定到能力系统组件的AbilityStatusChanged委托
	GetAuraASC()->AbilityStatusChanged.AddLambda
	([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
			{
				//当选中的能力状态发生变化时，立即更新UI按钮状态, 为了保证状态同步完整性：确保任何可能影响按钮状态的变化都会触发UI更新（选择法术球时，能力状态变化时，法术点变化时）
				SelectedAbility.Status = StatusTag;
				bool bEnableSpendPointsButton = false;// 初始化花费点数按钮状态
				bool bEnableEquipButton = false;// 初始化装备按钮状态
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// 调用辅助函数确定按钮状态
				SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton); // 广播按钮状态给UI

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
			CurrentSpellPoints = SpellPoints;

			//作用：当法术点数变化时，重新评估按钮状态。为了保证状态同步完整性：确保任何可能影响按钮状态的变化都会触发UI更新（选择法术球时，能力状态变化时，法术点变化时）
			//即使能力状态没变，点数变化可能改变按钮可用性
			//例如：点数从0变为1，现在可以花费点数升级能力

			bool bEnableSpendPointsButton = false;// 初始化花费点数按钮状态
			bool bEnableEquipButton = false;// 初始化装备按钮状态
			ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// 调用辅助函数确定按钮状态
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton); // 广播按钮状态给UI
		}
	);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)// 处理法术球被选中的逻辑
{
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
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton); // 广播按钮状态给UI

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
