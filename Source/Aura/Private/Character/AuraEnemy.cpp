// Copyright Ivy


#include "Character/AuraEnemy.h"

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}



AAuraEnemy::AAuraEnemy()
{

	bReplicates = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::die()
{
	Super::die();
	SetLifeSpan(LifeSpan);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);


	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}


	if (UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda
		(
			[this](const FOnAttributeChangeData& Data) 
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda
		(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

		//RegisterGameplayTagEvent �� ASC �ṩ�ķ��������� ע�����ĳ�� Gameplay Tag �ı仯��
	    //NewOrRemoved:���� Tag �� ��ӣ�Added�� �� �Ƴ���Removed�� ʱ���ᴥ���ص���
		//�ַ�ʽ�� GAS �зǳ����������� ��̬��Ӧ��Ϸ״̬�仯�����ܻ���Buff/Debuff�����ܴ����ȡ�
		AbilitySystemComponent->RegisterGameplayTagEvent
		(
			FAuraGameplayTags::Get().Effects_HitReact, // ������ Gameplay Tag
			EGameplayTagEventType::NewOrRemoved  // �������ͣ��������Ƴ���
		)
			.AddUObject
		(
			this,                                    // �ص�����this��
			&AAuraEnemy::HitReactTagChanged          // �ص�����
		);

		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
	}

}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed ? 0.f : BaseWalkSpeed;
}

void AAuraEnemy::InitAbilityActorInfo()
{
	//InitAbilityActorInfo() �������������ڳ�ʼ��ĳ����Ϸʵ�壨���ɫ����ߣ���������Ϣ�ġ�
    // ����ܰ������ø�ʵ��ļ��ܡ����ԡ���������������������ص����ݡ�
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefualtAttributes();
}

void AAuraEnemy::InitializeDefualtAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}
