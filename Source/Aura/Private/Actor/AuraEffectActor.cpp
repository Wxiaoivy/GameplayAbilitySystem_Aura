// Copyright Ivy


#include "Actor/AuraEffectActor.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 设置这个Actor不需要每帧都调用Tick函数，以提高性能
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}


// ApplyEffectToTarget函数的实现，用于向目标应用一个游戏效果
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// 获取目标的能力系统组件
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	// 如果目标没有能力系统组件，则直接返回
	if (TargetASC == nullptr)return;

	// 检查GameplayEffectClass是否有效
	check(GameplayEffectClass);

	// 创建一个游戏效果上下文句柄
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();

	// 将当前对象添加到效果上下文中作为来源
	EffectContextHandle.AddSourceObject(this);

	// 使用游戏效果类、强度（1.0f）和上下文句柄创建一个游戏效果规范句柄
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);

	// 将游戏效果规范应用到目标自身
	// EffectSpecHandle 是一个 句柄，这个句柄用于管理游戏效果规范(GameplayEffectSpec)的生命周期，并确保在适当的时候创建和销毁规范。
	// 当你调用 EffectSpecHandle.Data.Get() 时，你实际上是在请求获取 FGameplayEffectSpecHandle 内部存储的 FGameplayEffectSpec 实例的指针。
	// 由于 Get() 方法返回一个指向 FGameplayEffectSpec 实例的裸指针（FGameplayEffectSpec*），
	// 而 ApplyGameplayEffectSpecToSelf 需要的是一个引用（&），因此你需要使用解引用操作符（*）来将指针转换为对应的实例。
	const FActiveGameplayEffectHandle ActiveEffectHandle=TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}




}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}


	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}


	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	// 如果无限效果移除策略是“在结束重叠时移除”
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{

		// 从目标角色（Actor）获取能力系统组件（Ability System Component, ASC）
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		
		// 如果获取到的目标ASC无效（即不存在或者无法正常工作），则直接返回，不执行后续操作
		if (!IsValid(TargetASC))return;

		// 定义一个数组，用于存储需要移除的效果句柄（Handle）
		TArray<FActiveGameplayEffectHandle>HandlesToRemove;

		// 遍历所有活跃的效果句柄及其对应的ASC
		for (TPair<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			// 如果当前遍历到的ASC正是我们要操作的目标ASC
			if (TargetASC == HandlePair.Value)
			{
				// 从目标ASC中移除对应的效果(这里移除的是ASC的效果！！遍历的是TMap中的句柄，所以本质上没有改变TMap的数据结构）
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);
				// 将已移除的效果句柄添加到待移除数组中
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		// 遍历待移除数组中的每一个效果句柄
		for (auto& Handle : HandlesToRemove)
		{
			// 从活跃效果句柄集合中查找并移除该句柄（这里移除的是TMap中的效果句柄！！遍历的是待删除数组中的元素，所以本质上也没有修改数组中的元素）
		    // FindAndRemoveChecked确保句柄一定存在，如果不存在则会引发断言错误
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
		
	}
}


