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
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);//同一个Character里只能有一个Component去overlap一个GameplayEffect;(Mesh/Capsule)
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


//	//得到武器上面发射抛射物的Socket的位置
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
	// 创建一个游戏效果上下文句柄
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	// 将当前对象添加到效果上下文中作为来源
	ContextHandle.AddSourceObject(this);
	// 使用游戏效果类、强度（1.0f）和上下文句柄创建一个游戏效果规范句柄
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect, level, ContextHandle);
	// 将游戏效果规范应用到目标
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefualtAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttribute,1.0f);
	ApplyEffectToSelf(DefaultSecondaryAttribute, 1.0f);
	ApplyEffectToSelf(DefaultVitalAttribute, 1.0f);
}

//这个函数在AuraCharacter.cpp中的PossessedBy中调用 
void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked< UAuraAbilitySystemComponent>(AbilitySystemComponent);

	if (!HasAuthority())return;
	//这里调用AuraAbilitySystemComponent的AddCharacterAbilities（）这个函数。
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
	//由服务器触发
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
	//MulticastHandleDeath 是一个由服务器触发，同步到所有客户端的可靠多播 RPC，用于处理角色死亡后的全局逻辑。
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

	AbilitySystemComponent->RemoveLooseGameplayTag(FAuraGameplayTags::Get().Debuff_Burn);//AI加的  因为死亡时DebuffTag没有消除所以不触发UDebuffNiagaraComponent::DebuffTagChanged 这里加了就好了
	Dissolve();
	bIsDead = true;
	BurnNiagaraComponent->Deactivate();
	StunNiagaraComponent->Deactivate();
	OnDeathDelegate.Broadcast(this);


}

void AAuraCharacterBase::OnRep_Stunned()
{
	//老师把这个逻辑写在AuraCharacter的OnRep_Stunned里面，但是敌人没写，敌人又可以莫名其妙的激活该特效，
	//我觉得应该写在这里 敌人触发特效的逻辑才是正确的，
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
	if (bIsBurn)//这个布尔值都没设置呢，老师就写这个，无大语
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


