// Copyright Ivy


#include "AuraCharacterBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);//同一个Character里只能有一个Component去overlap一个GameplayEffect;(Mesh/Capsule)
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon -> SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}


// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();

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
}



UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::die()
{
	//由服务器触发
	//Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
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

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{

	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	//MulticastHandleDeath 是一个由服务器触发，同步到所有客户端的可靠多播 RPC，用于处理角色死亡后的全局逻辑。
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();
	bIsDead = true;

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


