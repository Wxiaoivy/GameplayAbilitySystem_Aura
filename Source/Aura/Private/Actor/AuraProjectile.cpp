// Copyright Ivy


#include "Actor/AuraProjectile.h"


AAuraProjectile::AAuraProjectile()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//发射物需要在服务器生成并复制到到所有客户端

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}


void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereBeginOverlap);

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//bHit = true; 的作用是标记投射物已经发生了碰撞。(详见有道云笔记数据传递和网络同步)
	// 由于网络同步的问题，客户端可能会在触发 OnSphereBeginOverlap 之前收到服务器的 Destroy() 调用，导致 OnSphereBeginOverlap 没有被执行。
	// 因此，bHit 的设计是为了确保即使客户端提前销毁了 Actor，也能正确处理碰撞逻辑。
	bHit = true;
	LoopingSoundComponent->Stop();
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	if (HasAuthority())
	{
		//获取TargetASC:
		//可以遍历 Overlap 结果，获取所有受影响的 Actor 的 ASC：
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		//Destroy()这是一个主动调用的函数，用于销毁 AActor 实例。
		//调用 Destroy() 后，引擎会自动调用 Destroyed()。Destroyed();
		Destroy();
		
	}
	/*else
	{
		bHit = true;
	}*/
}

void AAuraProjectile::Destroyed()
{

	if (!bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

