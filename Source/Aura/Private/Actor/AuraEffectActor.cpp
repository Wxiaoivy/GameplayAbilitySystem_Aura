// Copyright Ivy


#include "Actor/AuraEffectActor.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// �������Actor����Ҫÿ֡������Tick���������������
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RunningTime += DeltaSeconds;
	const float SinePeriod = 2 * PI / SinePeriodConstant; //��������ʱ�� = (2 * PI) / SinePeriodConstant
	if (RunningTime > SinePeriod)
	{
		RunningTime = 0.f;
	}
	ItemMovement(DeltaSeconds);
}

void AAuraEffectActor::ItemMovement(float DeltaTime)
{
	// ��� bRotates ���������Ƿ�Ϊ true
    // ���Ϊ true����ʾ�������Ӧ����ת
	if (bRotates)
	{
		// ����һ����ת�� DeltaRotation����ʾ��һ֡Ӧ����ת�ĽǶ�
	    // FRotator(Pitch, Yaw, Roll) - �ֱ��Ӧ��X��Y��Z�����ת
	    // ����ֻ��Yaw�ᣨY�ᣩ����ת����ת�ٶ�Ϊ RotationRate * DeltaTime
		const FRotator DeltaRotation(0.f, DeltaTime + RotationRate, 0.f);

		// ���µ���ת�� DeltaRotation ���ӵ���ǰ�ļ�����תֵ CalcultedRotation ��
        // ʵ����ת���ۻ�Ч��
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation); //UKismetMathLibrary::ComposeRotators()���ã����������ת�����൱����ѧ�ϵ���ת����
	}

	// ��� bSinusoidalMovement ���������Ƿ�Ϊ true
    // ���Ϊ true����ʾ�������Ӧ�ý������Ҳ��˶�
	if (bSinusoidalMovement)
	{
		// �������Һ�����ֵ����� * sin(ʱ�� * ���ڳ���)
	    // ������һ���� [-SineAmplitude, +SineAmplitude] ֮�������񵴵�ֵ
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodConstant);
		// �ڳ�ʼλ�õĻ����ϣ�ֻ��Z����������Ҳ��������ƫ����
        // ʵ�������ڴ�ֱ�����ϵ��������˶�
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, Sine);
	}
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();
}


void AAuraEffectActor::StartRotation()
{
	bRotates = true;
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
}

// ApplyEffectToTarget������ʵ�֣�������Ŀ��Ӧ��һ����ϷЧ��
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	
	bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	if (bIsEnemy && !bApplyEffectOnEnemies) return;  //���ȷ���ǵ��� ���Ҳ���ҪӦ��Effect��ֱ������
	


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
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);

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

	if (!bIsInfinite)
	{
		Destroy();
	}


}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{


	bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	if (bIsEnemy && !bApplyEffectOnEnemies) return;  //���ȷ���ǵ��� ���Ҳ���ҪӦ��Effect��ֱ������

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


	bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	if (bIsEnemy && !bApplyEffectOnEnemies) return;  //���ȷ���ǵ��� ���Ҳ���ҪӦ��Effect��ֱ������


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



