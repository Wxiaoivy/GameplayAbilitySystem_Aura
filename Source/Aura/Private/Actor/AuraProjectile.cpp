// Copyright Ivy


#include "Actor/AuraProjectile.h"


AAuraProjectile::AAuraProjectile()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//��������Ҫ�ڷ��������ɲ����Ƶ������пͻ���

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
	//bHit = true; �������Ǳ��Ͷ�����Ѿ���������ײ��(����е��Ʊʼ����ݴ��ݺ�����ͬ��)
	// ��������ͬ�������⣬�ͻ��˿��ܻ��ڴ��� OnSphereBeginOverlap ֮ǰ�յ��������� Destroy() ���ã����� OnSphereBeginOverlap û�б�ִ�С�
	// ��ˣ�bHit �������Ϊ��ȷ����ʹ�ͻ�����ǰ������ Actor��Ҳ����ȷ������ײ�߼���
	bHit = true;
	LoopingSoundComponent->Stop();
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	if (HasAuthority())
	{
		//��ȡTargetASC:
		//���Ա��� Overlap �������ȡ������Ӱ��� Actor �� ASC��
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		//Destroy()����һ���������õĺ������������� AActor ʵ����
		//���� Destroy() ��������Զ����� Destroyed()��Destroyed();
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

