// Copyright Ivy


#include "UI/WidgetController/OverlayWidgetController.h"
#include "Player/AuraPlayerState.h"


void UOverlayWidgetController::BroadCastInitialValues()
{
	//Super::BroadCastInitialValues();//这里可以不要Super  因为父类的实现部分是空白的

	

	//广播代理，动态多播代理的绑定在蓝图中实现的（绑定在血条和蓝条的WBP中） 先绑定后广播
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	//Super::BindCallbacksToDependencies();//这里可以不要Super  因为父类的实现部分是空白的


	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChange);
	AuraPlayerState->OnLevelChangedDelegate.AddLambda
	(                    [this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);

	//const UAuraAttributeSet* AuraAttributeSet = CastChecked< UAuraAttributeSet>(AttributeSet);



	//我们正在获取一个用于监听当前生命值属性变化的委托。并将Lambda绑定到之前获取的委托上
	//GetGameplayAttributeValueChangeDelegate（）是AbilitySystemComponent自带的一个函数。当有属性变化时，该代理会自动触发
	//该代理是一个多播代理（非动态）所以不需要用AddDynamic,就直接用AddUObject或者AddLambda。
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(GetAuraAS()->GetHealthAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	//我们正在获取一个用于监听最大生命值属性变化的委托。并将特定的函数MaxHealthChanged绑定到之前获取的委托上
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetManaAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetMaxManaAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	if (GetAuraASC())
	{
		//根据技能系统是否完成初始化（bStartupAbilitiesGiven），选择立即更新UI或等待初始化完成后再更新。
		if (GetAuraASC()->bStartupAbilitiesGiven)//检查 AuraASC 的 bStartupAbilitiesGiven 标志是否为 true,bStartupAbilitiesGiven 是 UAuraAbilitySystemComponent 中的一个布尔成员变量，
			                                //用于标记是否已经初始化了角色的初始技能（Startup Abilities）。如果为 true，说明技能已经初始化完成。
         {
			 BroadcastAbilityInfo();//如果技能已初始化，直接调用 OnInitializeStartupAbilities() 函数。这个函数可能是用来更新 UI（如技能栏、冷却时间显示等）或其他后续逻辑。
         }
		 else
		 {
			 //如果技能未初始化（bStartupAbilitiesGiven 为 false），则通过 AbilitiesGivenDelegate 绑定一个委托。
			 //当 AuraASC 完成技能初始化后，会自动触发 OnInitializeStartupAbilities。
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		 }



		GetAuraASC()->EffectAssetTag.AddLambda
		  (
			[this](const FGameplayTagContainer& AssetTags)//Lambda表达式里，如果想调用某个类的成员函数，必须在方括号里捕获该类，
			//这里就是调用本类的函数GetDataTableRowByTag（）所以方括号里写this.
			{
				// 遍历AssetTags容器中的所有标签。
				for (const FGameplayTag& Tag : AssetTags)
				{
					//TODO:Broadcast the Tag to the WidgetController (TODO: 向WidgetController广播这个标签（这里暂时未实现，仅作提示）)。


					//"A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
					//"Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False

					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					// 这行代码检查当前遍历到的标签（Tag）是否匹配MessageTag。
					// 如果Tag是"Message"或其任何后缀扩展（如"Message.HealthPotion"），则条件为真。
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);//传递信息给绑定这个代理的类（广播）
					}
				}
			}
		  );

	}

	GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
}

//void UOverlayWidgetController::OnInitializeStartupAbilities()//当技能初始化完成后，遍历所有技能，收集其标签和输入信息，并通过动态多播委托 AbilityInfoDelegate 通知UI更新。
//{
//	if (!GetAuraASC()->bStartupAbilitiesGiven)return;// 确保技能已初始化
//
//
//	// 定义Lambda委托，处理每个技能
//	FForEachAbility BroadcastDelegate;
//	BroadcastDelegate.BindLambda
//	(   [this](const FGameplayAbilitySpec& AbilitySpec)
//		{
//			// 1. 根据技能标签查找UI所需的信息（如图标、描述）
//			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(GetAuraASC()->GetAbilityTagFormSpec(AbilitySpec));
//			// 2. 绑定输入标签
//			Info.InputTag = GetAuraASC()->GetInputTagFormSpec(AbilitySpec);
//			// 3. 广播到UI更新
//			AbilityInfoDelegate.Broadcast(Info);
//
//		}
//	);
//	// 遍历所有技能并执行委托
//	GetAuraASC()->ForEachAbility(BroadcastDelegate);
//}

void UOverlayWidgetController::OnXPChange(int32 NewXP) 
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo,Please fill out AuraPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num()-1;

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;

		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}
void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	//关键步骤：清空旧绑定 → 2. 设置新绑定 → 3. 通知UI更新。
	//第一次广播 (LastSlotInfo)：清空旧槽位。
	//第二次广播(Info)：更新新槽位。
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;// 标记旧槽位为“可装备”(这个我不太能理解 我觉得只是把这些技能槽只设为可装备和已装备两种。 可装备就是还没有技能在里面为Abilities_Status_Unlocked和Abilities_None
	LastSlotInfo.InputTag = PreviousSlot; // 记录旧槽位（如 "Input.LMB"）
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None; // 清空技能(在蓝图里把Abilities_None的法术球都设为透明，）
	AbilityInfoDelegate.Broadcast(LastSlotInfo);// 如果之前有技能绑定在 PreviousSlot（如鼠标左键原本绑定了火球术），现在需要 清空它。广播 LastSlotInfo 告诉UI：“这个槽位现在空了，可以显示默认图标”。

	/*PreviousSlot 如果为空：说明技能之前未被绑定到任何槽位，因此不需要清理旧绑定。   蓝图（EquipGlobe）里面在接收AbilityInfo时就会先判断Input是否匹配，这里为空所以不匹配， 就不会执行后续逻辑了。
		广播的 LastSlotInfo：虽然不需要清理，但代码仍会广播一个“伪”清除消息，其内容为：
		InputTag = PreviousSlot（空标签）
		AbilityTag = Abilities_None（显式标记“无技能”）
		StatusTag = Abilities_Status_Unlocked（槽位状态设为“可装备”）*/

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);// 查找技能数据
	Info.StatusTag = Status;// 更新状态（如 "Equipped"）
	Info.InputTag = Slot; // 更新绑定的输入槽（如 "Input.LMB"）
	AbilityInfoDelegate.Broadcast(Info);// 通知UI更新  更新新装备的技能信息（如显示火球术图标到鼠标左键栏位）。广播 Info 告诉UI：“这个槽位现在绑定了新技能”。
}
