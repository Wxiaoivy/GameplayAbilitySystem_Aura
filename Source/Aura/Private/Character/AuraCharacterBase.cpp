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



void AAuraCharacterBase::InitializeDefualtAttributes(TSubclassOf<UGameplayEffect> GameplayEffect, float level)
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

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}



