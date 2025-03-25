// Copyright Ivy


#include "AbilitySystem/AbilityTask/TargetDataUnderCursor.h"

UTargetDataUnderCursor* UTargetDataUnderCursor::CreatTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	// ʹ�� NewAbilityTask ����һ�� UTargetDataUnderCursor ʵ��
	UTargetDataUnderCursor* MyObj = NewAbilityTask<UTargetDataUnderCursor>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderCursor::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		// ����Ǳ�����ҿ��ƣ����� SendMouseCursorData ��ȡ������Ŀ������
		SendMouseCursorData();
	}
	else
	{
		// ����Ƿ������ˣ��ȴ�����Ŀ�����ݣ�TODO����ʵ�֣�

		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		//AbilityTargetDataSetDelegate ��һ��ί�У�������Ŀ�����ݱ����Ƶ�������ʱ�����ص�������
		//���������ǣ����ͻ��˷��͵�Ŀ�����ݵ��������ʱ��֪ͨ��ص� AbilityTask ���߼�����ִ�С� OnTargetDataReplicatedCallBack��Ŀ�����ݵ���������󴥷��Ļص�������
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderCursor::OnTargetDataReplicatedCallBack);
		
		//CallReplicatedTargetDataDelegatesIfSet ���ڼ���Ƿ��Ѿ���Ŀ�����ݱ����Ƶ��������������Ŀ�������Ѿ�������������󶨵�ί�С�
		//�����������Ѿ�������������©����
		//true����ʾĿ�������Ѿ��������ί���Ѿ��������� false����ʾĿ��������δ���
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

		if (!bCalledDelegate)
		{
			// ���û��Ŀ�����ݱ����ƣ���������Ϊ�ȴ�Զ���������
			SetWaitingOnRemotePlayerData();
		}

	}
}

void UTargetDataUnderCursor::SendMouseCursorData()
{
	// ��Ԥ�ⴰ�ڣ����ڿͻ���Ԥ��
	//����Ҫ���ÿͻ���Ԥ��Ĵ�����У�����һ�� FScopedPredictionWindow ʵ����
	// �� FScopedPredictionWindow ���������ڣ��ͻ��˿���ִ��Ԥ�������
	FScopedPredictionWindow PredictionWindow(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	//ͨ�����߼���ȡ������µ����н����FHitResult����
	// �����н����װ�� FGameplayAbilityTargetData_SingleTargetHit �У�
	// ����ӵ� FGameplayAbilityTargetDataHandle��
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	//FGameplayAbilityTargetDataHandle ��һ��ͨ�õ�Ŀ���������������ڴ洢�͹���Ŀ�����ݡ�
	FGameplayAbilityTargetDataHandle DataHandle;
	//FGameplayAbilityTargetData_SingleTargetHit �� FGameplayAbilityTargetData ��һ�����࣬ר�����ڴ洢����Ŀ������н����
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();

	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData( //ServerSetReplicatedTargetData���ڽ��ͻ��˵�Ŀ�����ݷ��͵�����������ȷ����������������ȷͬ��������������ʵ�ֿͻ��˺ͷ�����֮���Ŀ�����ݴ��ݣ�ͨ�����ڴ����ܵ�Ŀ��ѡ�����м����߼���
		GetAbilitySpecHandle(), //��ǰ GameplayAbility �ľ�������ڱ�ʶ�ĸ� Ability ������Ŀ�����ݡ�
		GetActivationPredictionKey(), //��ǰ GameplayAbility ��"����Ԥ���"�����ڱ�ʶ�ͻ��˵�Ԥ�������
		DataHandle, //Ŀ�����ݾ���������ͻ��˵�Ŀ�����ݣ������н����Ŀ��λ�õȣ���
		FGameplayTag(), //Ӧ�ñ�ǩ����ѡ�������ڴ��ݶ������Ϣ���缼�����͡�Ч�����͵ȣ���
		AbilitySystemComponent->ScopedPredictionKey//"��ǰԤ���"�����ڱ�ʶ�ͻ��˵�Ԥ�ⴰ��
	);

	//��Ŀ������׼���ú󣬵��� ValidData.Broadcast(DataHandle)���������а󶨵ĺ�����
	// Broadcast���Ƕಥί�еĴ��������������ε������а󶨵ĺ����������ݲ��� DataHandle��
	if (ShouldBroadcastAbilityTaskDelegates())//ShouldBroadcastAbilityTaskDelegates() �����жϵ�ǰ�Ƿ�Ӧ�ù㲥ί�С������ڲ����ʵ������´���ί�У������߼���������
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderCursor::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag Tag)
{
	//ConsumeClientReplicatedTargetData �������ѿͻ��˸��Ƶ�Ŀ�����ݣ�ȷ�����ݲ��ᱻ�ظ�����
	//���������ǣ����Ŀ�������Ѿ������������ظ�����ί�С�
	//��Ŀ�����ݱ�����󣬵��������������������Ѿ������ѡ�ͨ���ڻص������е��á�
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())//ShouldBroadcastAbilityTaskDelegates() �����жϵ�ǰ�Ƿ�Ӧ�ù㲥ί�С������ڲ����ʵ������´���ί�У������߼���������
	{
		ValidData.Broadcast(DataHandle);
	}
}


