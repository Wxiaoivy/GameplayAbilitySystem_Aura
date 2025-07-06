// Copyright Ivy


#include "Actor/AuraProjectile.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"


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

	//UGameplayStatics::SpawnSoundAttached
	//���� UE �ṩ��һ����̬�����������ڳ����в������������丽�ӣ�Attach����ĳ������ϡ�
    //�����������ӵ����ʱ��������������ƶ��������ɫ�ƶ�ʱ����Ҳ������ƶ�����
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
		}, 0.05f, false); // ��һִ֡��
}


void AAuraProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	// ��ֹͶ���Projectile���ԡ�����˺���Դͷ����Effect Causer����������˺���ͨ�����ڱ������˻��߼���ͻ�������
	//DamageEffectSpecHandle.Data.IsValid()  �ж��������ΪAuraProjectileSpell�����SpawnProjectile��������ж��Ƿ�Ϊ�������ˣ� 
	// ����ǿͻ��˾�ֱ�ӷ��� ����������DamageEffectSpecHandle���������������DamageEffectSpecHandle.Data����Ч�ģ�
	//��ΪActor�����ΪbReplicated = true��  Actor ���������ٺͱ任���ᱻ������������Զ���������
	if (DamageEffectParams.SourceAbilitySystemComponent)
	{
		AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
		if (SourceAvatarActor == OtherActor)return;

		//��鹥���ߣ�EffectCauser����Ŀ�꣨OtherActor���Ƿ����ѷ���ϵ��������ѷ�����ֱ�ӷ��أ�������˺���
		if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor))return;
	}
		
	//bHit = true; �������Ǳ��Ͷ�����Ѿ���������ײ��(����е��Ʊʼ����ݴ��ݺ�����ͬ��)
	// ��������ͬ�������⣬�ͻ��˿��ܻ��ڴ��� OnSphereBeginOverlap ֮ǰ�յ��������� Destroy() ���ã����� OnSphereBeginOverlap û�б�ִ�С�
	// ��ˣ�bHit �������Ϊ��ȷ����ʹ�ͻ�����ǰ������ Actor��Ҳ����ȷ������ײ�߼���
	if (!bHit && LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		bHit = true;
	}
	

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
		//Destroy()����һ���������õĺ������������� AActor ʵ����
		//���� Destroy() ��������Զ����� Destroyed()��
		Destroy();
		
	}
	else
	{
		//bHit = true ������ else ���У����ͻ����߼���֧����������ֱ�ӷ�����ײ���֮������Ϊ�� �ϸ����ַ������Ϳͻ��˵�ְ�𣬲�ȷ������ͬ������ȷ�ԡ�
		bHit = true;// �������bHit����ȥ��DeepSeek������   ��Unreal Engine�е�����ͬ�����֪ʶ�㣨P160����
	}
}

void AAuraProjectile::Destroyed()
{

	if (!bHit && !HasAuthority() && LoopingSoundComponent)//!bHit && !HasAuthority()
		                                                   //!bHit��˵���ͻ������յ�����֪ͨǰδ���� OnSphereBeginOverlap�������������ӳ٣���
		                                                   //!HasAuthority()�����ڿͻ���ִ�в����߼����������������������ײ����
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
		bHit = true;
	}
	Super::Destroyed();
}

