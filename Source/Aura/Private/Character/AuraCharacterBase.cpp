// Copyright Ivy


#include "AuraCharacterBase.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	return Weapon->GetSocketLocation(WeaponTipSocketName);
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



