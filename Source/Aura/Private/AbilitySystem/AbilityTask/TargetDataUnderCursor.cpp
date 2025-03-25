// Copyright Ivy


#include "AbilitySystem/AbilityTask/TargetDataUnderCursor.h"

UTargetDataUnderCursor* UTargetDataUnderCursor::CreatTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	// 使用 NewAbilityTask 创建一个 UTargetDataUnderCursor 实例
	UTargetDataUnderCursor* MyObj = NewAbilityTask<UTargetDataUnderCursor>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderCursor::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		// 如果是本地玩家控制，调用 SendMouseCursorData 获取并发送目标数据
		SendMouseCursorData();
	}
	else
	{
		// 如果是服务器端，等待接收目标数据（TODO：待实现）

		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		//AbilityTargetDataSetDelegate 是一个委托，用于在目标数据被复制到服务器时触发回调函数。
		//它的作用是：当客户端发送的目标数据到达服务器时，通知相关的 AbilityTask 或逻辑继续执行。 OnTargetDataReplicatedCallBack：目标数据到达服务器后触发的回调函数。
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderCursor::OnTargetDataReplicatedCallBack);
		
		//CallReplicatedTargetDataDelegatesIfSet 用于检查是否已经有目标数据被复制到服务器。如果有目标数据已经到达，立即触发绑定的委托。
		//避免在数据已经到达的情况下遗漏处理。
		//true：表示目标数据已经到达，并且委托已经被触发。 false：表示目标数据尚未到达。
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

		if (!bCalledDelegate)
		{
			// 如果没有目标数据被复制，设置任务为等待远程玩家数据
			SetWaitingOnRemotePlayerData();
		}

	}
}

void UTargetDataUnderCursor::SendMouseCursorData()
{
	// 打开预测窗口，用于客户端预测
	//在需要启用客户端预测的代码块中，创建一个 FScopedPredictionWindow 实例。
	// 在 FScopedPredictionWindow 的作用域内，客户端可以执行预测操作。
	FScopedPredictionWindow PredictionWindow(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	//通过射线检测获取鼠标光标下的命中结果（FHitResult）。
	// 将命中结果封装到 FGameplayAbilityTargetData_SingleTargetHit 中，
	// 并添加到 FGameplayAbilityTargetDataHandle。
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	//FGameplayAbilityTargetDataHandle 是一个通用的目标数据容器，用于存储和管理目标数据。
	FGameplayAbilityTargetDataHandle DataHandle;
	//FGameplayAbilityTargetData_SingleTargetHit 是 FGameplayAbilityTargetData 的一个子类，专门用于存储单个目标的命中结果。
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();

	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData( //ServerSetReplicatedTargetData用于将客户端的目标数据发送到服务器，并确保数据在网络中正确同步。它的作用是实现客户端和服务器之间的目标数据传递，通常用于处理技能的目标选择、命中检测等逻辑。
		GetAbilitySpecHandle(), //当前 GameplayAbility 的句柄，用于标识哪个 Ability 发送了目标数据。
		GetActivationPredictionKey(), //当前 GameplayAbility 的"激活预测键"，用于标识客户端的预测操作。
		DataHandle, //目标数据句柄，包含客户端的目标数据（如命中结果、目标位置等）。
		FGameplayTag(), //应用标签（可选），用于传递额外的信息（如技能类型、效果类型等）。
		AbilitySystemComponent->ScopedPredictionKey//"当前预测键"，用于标识客户端的预测窗口
	);

	//当目标数据准备好后，调用 ValidData.Broadcast(DataHandle)，触发所有绑定的函数。
	// Broadcast：是多播委托的触发方法，会依次调用所有绑定的函数，并传递参数 DataHandle。
	if (ShouldBroadcastAbilityTaskDelegates())//ShouldBroadcastAbilityTaskDelegates() 用于判断当前是否应该广播委托。避免在不合适的条件下触发委托，导致逻辑错误或崩溃
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderCursor::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag Tag)
{
	//ConsumeClientReplicatedTargetData 用于消费客户端复制的目标数据，确保数据不会被重复处理。
	//它的作用是：标记目标数据已经被处理，避免重复触发委托。
	//在目标数据被处理后，调用这个函数，标记数据已经被消费。通常在回调函数中调用。
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())//ShouldBroadcastAbilityTaskDelegates() 用于判断当前是否应该广播委托。避免在不合适的条件下触发委托，导致逻辑错误或崩溃
	{
		ValidData.Broadcast(DataHandle);
	}
}


