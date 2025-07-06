// Copyright Ivy


#include "Actor/AuraProjectile.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"


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

	//UGameplayStatics::SpawnSoundAttached
	//这是 UE 提供的一个静态函数，用于在场景中播放声音并将其附加（Attach）到某个组件上。
    //当声音被附加到组件时，它会跟随该组件移动（比如角色移动时声音也会跟着移动）。
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (IsValid(this) && LoopingSound && GetRootComponent())
			{
				LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(
					LoopingSound,
					GetRootComponent()
				);
			}
		}, 0.05f, false); // 下一帧执行
}


void AAuraProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	// 防止投射物（Projectile）对“造成伤害的源头”（Effect Causer）自身造成伤害，通常用于避免自伤或逻辑冲突的情况。
	//DamageEffectSpecHandle.Data.IsValid()  判断这个是因为AuraProjectileSpell类里的SpawnProjectile函数里会判断是否为服务器端， 
	// 如果是客户端就直接返回 并不会生成DamageEffectSpecHandle，所以这种情况下DamageEffectSpecHandle.Data是无效的，
	//因为Actor被标记为bReplicated = true后  Actor 的生成销毁和变换都会被引擎服务器端自动处理并复制
	if (DamageEffectParams.SourceAbilitySystemComponent)
	{
		AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
		if (SourceAvatarActor == OtherActor)return;

		//检查攻击者（EffectCauser）和目标（OtherActor）是否是友方关系。如果是友方，则直接返回，不造成伤害。
		if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor))return;
	}
		
	//bHit = true; 的作用是标记投射物已经发生了碰撞。(详见有道云笔记数据传递和网络同步)
	// 由于网络同步的问题，客户端可能会在触发 OnSphereBeginOverlap 之前收到服务器的 Destroy() 调用，导致 OnSphereBeginOverlap 没有被执行。
	// 因此，bHit 的设计是为了确保即使客户端提前销毁了 Actor，也能正确处理碰撞逻辑。
	if (!bHit && LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		bHit = true;
	}
	

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
			const bool bKnockBack = FMath::RandRange(1, 100) < DamageEffectParams.KnockBackChance;
			if (bKnockBack)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;

				const FVector KnockBackDirection = Rotation.Vector();
				const FVector KnockBackForce = KnockBackDirection * DamageEffectParams.KnockBackMagnitude;
				DamageEffectParams.KnockBackForce = KnockBackForce;
			}


			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		//Destroy()这是一个主动调用的函数，用于销毁 AActor 实例。
		//调用 Destroy() 后，引擎会自动调用 Destroyed()。
		Destroy();
		
	}
	else
	{
		//bHit = true 被放在 else 块中（即客户端逻辑分支），而不是直接放在碰撞检测之后，这是为了 严格区分服务器和客户端的职责，并确保网络同步的正确性。
		bHit = true;// 关于这个bHit具体去看DeepSeek搜索的   “Unreal Engine中的网络同步相关知识点（P160）”
	}
}

void AAuraProjectile::Destroyed()
{

	if (!bHit && !HasAuthority() && LoopingSoundComponent)//!bHit && !HasAuthority()
		                                                   //!bHit：说明客户端在收到销毁通知前未触发 OnSphereBeginOverlap（可能因网络延迟）。
		                                                   //!HasAuthority()：仅在客户端执行补播逻辑（服务器已正常处理过碰撞）。
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
		bHit = true;
	}
	Super::Destroyed();
}

