// Copyright Ivy


#include "Actor/AuraFireBall.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"


AAuraFireBall::AAuraFireBall()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//��������Ҫ�ڷ��������ɲ����Ƶ������пͻ���
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();

	StartOutGoingTimeline();
}

void AAuraFireBall::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!IsValidOverlap(OtherActor))return;

	//bHit = true; �������Ǳ��Ͷ�����Ѿ���������ײ��(����е��Ʊʼ����ݴ��ݺ�����ͬ��)
	// ��������ͬ�������⣬�ͻ��˿��ܻ��ڴ��� OnSphereBeginOverlap ֮ǰ�յ��������� Destroy() ���ã����� OnSphereBeginOverlap û�б�ִ�С�
	// ��ˣ�bHit �������Ϊ��ȷ����ʹ�ͻ�����ǰ������ Actor��Ҳ����ȷ������ײ�߼���

	if (HasAuthority())//HasAuthority()��鵱ǰ�Ƿ�Ϊ��������Authority����ֻ�з������ܴ����˺��߼������� Actor���ͻ�����ȴ�������ͬ����
	{
		//��ȡTargetASC:
		//����DamageEffectParams�����TargetAbilitySystemComponent
		//����DamageEffectParams�����DeathImpuse(������������)
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpuse = GetActorForwardVector() * DamageEffectParams.DeathImpuseMagnitude;
			DamageEffectParams.DeathImpuse = DeathImpuse;
			//�����ú�DamageEffectParams.DeathImpuse ��Ȼ����ApplyDamageEffect����������᷵�ش�������������ContextSpecHandle,Handle������ݾͻᴫ�ݵ�AttributeSet����ʹ��

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
