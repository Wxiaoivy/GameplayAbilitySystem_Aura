// Copyright Ivy


#include "AbilitySystem/Abilities/AuraPassiveAbility.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemBlueprintLibrary.h>

void UAuraPassiveAbility::ActivateAbility
(
	const FGameplayAbilitySpecHandle Handle,                // 技能实例的唯一标识
	const FGameplayAbilityActorInfo* ActorInfo,             // 拥有该技能的Actor信息
	const FGameplayAbilityActivationInfo ActivationInfo,    // 激活信息
	const FGameplayEventData* TriggerEventData              // 触发事件数据(如果有)
)
{
	// 调用父类的ActivateAbility方法进行基础激活操作
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 尝试获取AvatarActor的能力系统组件(ASC)并转换为UAuraAbilitySystemComponent类型
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		// 将ReceivedDeactivate方法绑定到ASC的DeactivatePassiveAbility委托上
		// 当ASC想要停用被动技能时，会调用这个委托
		AuraASC->DeactivatePassiveAbility.AddUObject(this, &UAuraPassiveAbility::ReceivedDeactivate);
	}

}
//功能说明
//ActivateAbility :

//当被动技能激活时调用
//主要功能是获取角色的能力系统组件并绑定停用委托
//当需要停用被动技能时，能力系统组件会通过这个委托通知技能

//ReceivedDeactivate :

//当接收到停用请求时调用
//检查请求停用的技能标签是否匹配当前技能
//如果匹配，则结束当前技能
// 
// 
//这段代码主要用于实现被动技能的基本框架，允许被动技能被外部系统(如能力系统组件)通过标签匹配来停用。

void UAuraPassiveAbility::ReceivedDeactivate(const FGameplayTag& AbilityTag)// 接收停用被动技能请求
{
	// 检查传入的技能标签是否与当前技能的标签完全匹配
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		// 如果匹配，则结束当前技能
			// 参数说明:
			// - CurrentSpecHandle: 当前技能实例的句柄
			// - CurrentActorInfo: 当前Actor信息
			// - CurrentActivationInfo: 当前激活信息
			// - true: 表示是否触发复制
			// - true: 表示技能是否成功结束
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
