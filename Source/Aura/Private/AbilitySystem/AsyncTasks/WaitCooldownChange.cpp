// Copyright Ivy


	#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"



	UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbillitySystemComponent, const FGameplayTag& InCooldownTag)
	{
		UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();//创建一个新的 UWaitCooldownChange 实例。
		WaitCooldownChange->ASC = AbillitySystemComponent;//保存技能系统组件，后续用来监听事件。
		WaitCooldownChange->CooldownTag = InCooldownTag;//保存要监听的冷却标签。

		if (!IsValid(AbillitySystemComponent)|| !InCooldownTag.IsValid())
		{
			WaitCooldownChange->EndTask();
			return nullptr;
		}


		//当冷却是由 GameplayEffect（GE） 驱动时,使用 OnActiveGameplayEffectAddedDelegateToSelf,✅ 精确获取冷却时间✅ 处理复杂冷却逻辑✅ 兼容更多冷却实现方式
		//为什么不能只用 if (NewCount == 1) ❌ 无法直接获取剩余时间❌ 无法处理GE驱动的冷却❌ 难以处理叠加冷却

		/*典型工作流程
			冷却开始：

			通过 OnActiveEffectAdded 检测GE添加 → 计算时间 → 广播 CooldownStart

			冷却结束：

			通过 CooldownTagChange 检测标签移除 → 广播 CooldownEnd
			（即使GE时间计算出错，标签变化也能兜底）*/

		/*优先使用 OnActiveGameplayEffectAdded
		适用于大多数情况，能获取最完整的冷却信息。

		用标签事件作为补充(RegisterGameplayTagEvent)
		处理GE意外的冷却结束情况（如被强制移除）*/

		/*冷却开始时能拿到准确时间(OnActiveGameplayEffectAddedDelegateToSelf)

		冷却结束时一定能被检测到(RegisterGameplayTagEvent)*/
		AbillitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);
		AbillitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChange);
		//OnActiveGameplayEffectAddedDelegateToSelf：效果成功应用并激活后触发(一般用于判断冷却开始）
		//使用 RegisterGameplayTagEvent 监听标签计数变化（一般用于判断冷却结束）
		return WaitCooldownChange;
	}

	void UWaitCooldownChange::EndTask()//永远记得清理委托   在 EndTask() 中移除所有绑定，防止内存泄漏。
	{
		if (!IsValid(ASC))return;
		ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

		//SetReadyToDestroy() 和 MarkAsGarbage() 是用于 对象生命周期管理 的关键方法。它们在冷却任务结束时被调用，目的是安全清理内存。
		SetReadyToDestroy();//SetReadyToDestroy():标记该异步任务对象为 "准备销毁" 状态。通知 UE 的垃圾回收系统（Garbage Collection, GC）这个对象已经不再需要。
		MarkAsGarbage();//MarkAsGarbage():强制将对象标记为 "垃圾"，加速垃圾回收过程。与 SetReadyToDestroy() 不同，它会 立即 将对象推入 GC 系统，而不是等待下一轮回收。
	}

	void UWaitCooldownChange::CooldownTagChange(const FGameplayTag InCooldownTag, int32 NewCount)
	{
		if (NewCount == 0)//标签的当前数量（0 表示冷却结束）。
		{
			CooldownEnd.Broadcast(0.f);//触发 CooldownEnd 委托，通知所有绑定的函数。
		}

	}


	void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
	{
		FGameplayTagContainer AssetTags;
		SpecApplied.GetAllAssetTags(AssetTags);// 获取技能效果自带的标签

		FGameplayTagContainer GrantedTag;
		SpecApplied.GetAllGrantedTags(GrantedTag);// 获取技能效果赋予的标签

		if (AssetTags.HasTagExact(CooldownTag)|| GrantedTag.HasTagExact(CooldownTag))
		{
			FGameplayEffectQuery EffectQuery = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
			TArray<float>TimesRemaining = ASC->GetActiveEffectsTimeRemaining(EffectQuery);// 获取剩余时间的数组


			 if (TimesRemaining.Num() > 0)
			 {
				 float TimeRemaining = TimesRemaining[0];
				 for (int i = 0; i < TimesRemaining.Num(); i++)
				 {
					 if (TimesRemaining[i]> TimeRemaining)
					 {
						 TimeRemaining = TimesRemaining[i];// 取最大值（最长的冷却时间）
					 }
				 }
				 CooldownStart.Broadcast(TimeRemaining); // 通知冷却开始
			 }
		}
	
	}

