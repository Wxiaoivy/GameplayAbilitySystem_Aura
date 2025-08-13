// Copyright Ivy


#include "Actor/AuraFireBall.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"


AAuraFireBall::AAuraFireBall()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//发射物需要在服务器生成并复制到到所有客户端
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();

	StartOutGoingTimeline();
}

void AAuraFireBall::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!IsValidOverlap(OtherActor))return;

	//bHit = true; 的作用是标记投射物已经发生了碰撞。(详见有道云笔记数据传递和网络同步)
	// 由于网络同步的问题，客户端可能会在触发 OnSphereBeginOverlap 之前收到服务器的 Destroy() 调用，导致 OnSphereBeginOverlap 没有被执行。
	// 因此，bHit 的设计是为了确保即使客户端提前销毁了 Actor，也能正确处理碰撞逻辑。

	if (HasAuthority())//HasAuthority()检查当前是否为服务器（Authority）。只有服务器能处理伤害逻辑和销毁 Actor，客户端需等待服务器同步。
	{
		//获取TargetASC:
		//设置DamageEffectParams里面的TargetAbilitySystemComponent
		//设置DamageEffectParams里面的DeathImpuse(死亡冲量向量)
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpuse = GetActorForwardVector() * DamageEffectParams.DeathImpuseMagnitude;
			DamageEffectParams.DeathImpuse = DeathImpuse;
			//先设置好DamageEffectParams.DeathImpuse ，然后在ApplyDamageEffect（）函数里会返回带有死亡冲量的ContextSpecHandle,Handle里的数据就会传递到AttributeSet里面使用

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
