// Copyright Ivy


#include "AuraCharacterBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <../../../../../../../Source/Runtime/Engine/Public/Net/UnrealNetwork.h>


// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BurnNiagaraComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnNiagaraComponent");
	BurnNiagaraComponent->SetupAttachment(GetRootComponent());
	BurnNiagaraComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;

	StunNiagaraComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunNiagaraComponent");
	StunNiagaraComponent->SetupAttachment(GetRootComponent());
	StunNiagaraComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;
	

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);//ͬһ��Character��ֻ����һ��Componentȥoverlapһ��GameplayEffect;(Mesh/Capsule)
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon -> SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachComponent");
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	HaloOfProtectionNiagaraSystemComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionNiagaraSystemComponent");
	HaloOfProtectionNiagaraSystemComponent->SetupAttachment(EffectAttachComponent);
	LifeSiphonNiagaraSystemComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonNiagaraSystemComponent");
	LifeSiphonNiagaraSystemComponent->SetupAttachment(EffectAttachComponent);
	ManaSiphonNiagaraSystemComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonNiagaraSystemComponent");
	ManaSiphonNiagaraSystemComponent->SetupAttachment(EffectAttachComponent);

}


void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurn);
	DOREPLIFETIME(AAuraCharacterBase, bIsBeingShocked);
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}


//	//�õ��������淢���������Socket��λ��
FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon)&&IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffect, float level)const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffect);
	// ����һ����ϷЧ�������ľ��
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	// ����ǰ������ӵ�Ч������������Ϊ��Դ
	ContextHandle.AddSourceObject(this);
	// ʹ����ϷЧ���ࡢǿ�ȣ�1.0f���������ľ������һ����ϷЧ���淶���
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect, level, ContextHandle);
	// ����ϷЧ���淶Ӧ�õ�Ŀ��
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefualtAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttribute,1.0f);
	ApplyEffectToSelf(DefaultSecondaryAttribute, 1.0f);
	ApplyEffectToSelf(DefaultVitalAttribute, 1.0f);
}

//���������AuraCharacter.cpp�е�PossessedBy�е��� 
void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked< UAuraAbilitySystemComponent>(AbilitySystemComponent);

	if (!HasAuthority())return;
	//�������AuraAbilitySystemComponent��AddCharacterAbilities�������������
	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}



UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::die(const FVector& DeathImpuse)
{
	//�ɷ���������
	//Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpuse);

}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnASCRegisteredDelegate;
}

FOnDeathSignature& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

void AAuraCharacterBase::SetIsBeingShocked_Implementation(bool InIsBeingShocked)
{
	bIsBeingShocked = InIsBeingShocked;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation()
{
	return bIsBeingShocked;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::IncrementalMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage:AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpuse)
{

	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	//MulticastHandleDeath ��һ���ɷ�����������ͬ�������пͻ��˵Ŀɿ��ಥ RPC�����ڴ����ɫ�������ȫ���߼���
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	Weapon->AddImpulse(DeathImpuse * 0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpuse, NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySystemComponent->RemoveLooseGameplayTag(FAuraGameplayTags::Get().Debuff_Burn);//AI�ӵ�  ��Ϊ����ʱDebuffTagû���������Բ�����UDebuffNiagaraComponent::DebuffTagChanged ������˾ͺ���
	Dissolve();
	bIsDead = true;
	BurnNiagaraComponent->Deactivate();
	StunNiagaraComponent->Deactivate();
	OnDeathDelegate.Broadcast(this);


}

void AAuraCharacterBase::OnRep_Stunned()
{
	//��ʦ������߼�д��AuraCharacter��OnRep_Stunned���棬���ǵ���ûд�������ֿ���Ī������ļ������Ч��
	//�Ҿ���Ӧ��д������ ���˴�����Ч���߼�������ȷ�ģ�
	if (bIsStunned)
	{
		StunNiagaraComponent->Activate();
	}
	else
	{
		StunNiagaraComponent->Deactivate();
	}
}

void AAuraCharacterBase::OnRep_Burn()
{
	if (bIsBurn)//�������ֵ��û�����أ���ʦ��д������޴���
	{
		BurnNiagaraComponent->Activate();
	} 
	else
	{
		BurnNiagaraComponent->Deactivate();
	}
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		StartDissolveTimeline(DynamicMatInst);
	}

	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);
		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

bool AAuraCharacterBase::IsDead_Implementation()
{
	return bIsDead;
}


