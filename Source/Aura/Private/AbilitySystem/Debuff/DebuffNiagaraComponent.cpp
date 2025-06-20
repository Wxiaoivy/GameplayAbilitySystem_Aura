// Copyright Ivy


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Interaction/CombatInterface.h"
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemBlueprintLibrary.h>
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemComponent.h>

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;// 在构造函数中设置 bAutoActivate 为 false，表示这个 Niagara 粒子效果不会自动播放，需要根据游戏逻辑手动激活。
}

void UDebuffNiagaraComponent::BeginPlay()
{
	/*这个函数在游戏开始时调用，设置各种事件监听
		首先尝试获取拥有者的能力系统组件(ASC)
		如果ASC存在，立即注册对特定游戏标签(DebuffTag)变化的回调
		如果ASC还不存在但拥有战斗接口，设置一个lambda函数，等ASC注册后再添加回调
		最后注册死亡事件的回调*/
	
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());// 尝试获取拥有者的战斗接口
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		// 如果成功获取ASC，注册对DebuffTag标签变化的回调
		//这里明确指定了要监听的是 DebuffTag 这个特定标签的变化
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if (CombatInterface)
	{
		// 如果当前没有ASC但拥有战斗接口，设置一个委托，当ASC注册时再添加标签回调
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda//这里用不了AddWeakLambda要报错,长期委托，需弱引用支持 您的场景属于 "短期委托 + 对象生命周期可控"，因此 AddLambda 更合适。
		(        [this](UAbilitySystemComponent* InASC)        //因为CombatInterface 通常是角色或组件的一部分，角色销毁时委托也会被释放。
			{
				InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
			}
		);
	}
	if (CombatInterface)
	{
		// 如果拥有战斗接口，注册死亡事件的回调
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}
//CallbackTag：实际触发变化的标签（就是你注册时指定的 DebuffTag） NewCount：该标签当前在ASC上的计数,虽然函数体里没用，但是表示了到底是检测哪个Tag的变化 这里的CallbackTag=DebuffTag
void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		Activate();// 如果标签计数大于0，激活粒子效果
	}
	else
	{
		Deactivate();// 否则停用粒子效果
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();// 当拥有者死亡时停用粒子效果
}
