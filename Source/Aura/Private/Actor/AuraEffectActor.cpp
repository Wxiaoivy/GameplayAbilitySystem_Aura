// Copyright Ivy


#include "Actor/AuraEffectActor.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// �������Actor����Ҫÿ֡������Tick���������������
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}


// ApplyEffectToTarget������ʵ�֣�������Ŀ��Ӧ��һ����ϷЧ��
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// ��ȡĿ�������ϵͳ���
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	// ���Ŀ��û������ϵͳ�������ֱ�ӷ���
	if (TargetASC == nullptr)return;

	// ���GameplayEffectClass�Ƿ���Ч
	check(GameplayEffectClass);

	// ����һ����ϷЧ�������ľ��
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();

	// ����ǰ������ӵ�Ч������������Ϊ��Դ
	EffectContextHandle.AddSourceObject(this);

	// ʹ����ϷЧ���ࡢǿ�ȣ�1.0f���������ľ������һ����ϷЧ���淶���
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);

	// ����ϷЧ���淶Ӧ�õ�Ŀ������
	// EffectSpecHandle ��һ�� ��������������ڹ�����ϷЧ���淶(GameplayEffectSpec)���������ڣ���ȷ�����ʵ���ʱ�򴴽������ٹ淶��
	// ������� EffectSpecHandle.Data.Get() ʱ����ʵ�������������ȡ FGameplayEffectSpecHandle �ڲ��洢�� FGameplayEffectSpec ʵ����ָ�롣
	// ���� Get() ��������һ��ָ�� FGameplayEffectSpec ʵ������ָ�루FGameplayEffectSpec*����
	// �� ApplyGameplayEffectSpecToSelf ��Ҫ����һ�����ã�&�����������Ҫʹ�ý����ò�������*������ָ��ת��Ϊ��Ӧ��ʵ����
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

	// �������Ч���Ƴ������ǡ��ڽ����ص�ʱ�Ƴ���
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{

		// ��Ŀ���ɫ��Actor����ȡ����ϵͳ�����Ability System Component, ASC��
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		
		// �����ȡ����Ŀ��ASC��Ч���������ڻ����޷���������������ֱ�ӷ��أ���ִ�к�������
		if (!IsValid(TargetASC))return;

		// ����һ�����飬���ڴ洢��Ҫ�Ƴ���Ч�������Handle��
		TArray<FActiveGameplayEffectHandle>HandlesToRemove;

		// �������л�Ծ��Ч����������Ӧ��ASC
		for (TPair<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			// �����ǰ��������ASC��������Ҫ������Ŀ��ASC
			if (TargetASC == HandlePair.Value)
			{
				// ��Ŀ��ASC���Ƴ���Ӧ��Ч��(�����Ƴ�����ASC��Ч��������������TMap�еľ�������Ա�����û�иı�TMap�����ݽṹ��
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);
				// �����Ƴ���Ч�������ӵ����Ƴ�������
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		// �������Ƴ������е�ÿһ��Ч�����
		for (auto& Handle : HandlesToRemove)
		{
			// �ӻ�ԾЧ����������в��Ҳ��Ƴ��þ���������Ƴ�����TMap�е�Ч����������������Ǵ�ɾ�������е�Ԫ�أ����Ա�����Ҳû���޸������е�Ԫ�أ�
		    // FindAndRemoveCheckedȷ�����һ�����ڣ��������������������Դ���
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
		
	}
}


