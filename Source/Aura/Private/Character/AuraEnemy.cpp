// Copyright Ivy


#include "Character/AuraEnemy.h"


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


	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;//��Ҫ����ƽ��ת��
	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	GetMesh()->MarkRenderStateDirty();
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->MarkRenderStateDirty();

	BaseWalkSpeed = 250.f;

}


void AAuraEnemy::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}



//PossessedBy()�ǽ�ɫ���������ӹ�ʱ�Զ�����������ɫ��������������������� ���ᴥ����
void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())return;

	AuraAIController = Cast<AAuraAIController>(NewController);
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);

	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitRacting"), bHitReacting);
	
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}


void AAuraEnemy::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	//DO not change OutDestination
}

void AAuraEnemy::die(const FVector& DeathImpuse)
{
	SetLifeSpan(LifeSpan);
	if (AuraAIController)
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}
	SpawnLoot();
	Super::die(DeathImpuse);
	
}



void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement() && GetCharacterMovement()->HasBegunPlay())
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
	
	InitAbilityActorInfo();
	if (HasAuthority())
	{ 
		//GiveStartupAbilities �Ƿ�����Ȩ���߼���
		//BeginPlay �ڿͻ��˺ͷ�����������ã�����������ֻ���������ʼ��һ�Σ�Ȼ��ͨ������ͬ��

		//������� HasAuthority() �ķ���:
		//�ͻ��˿��᳢ܻ�Ը��Լ����輼�ܣ����޷�ͬ����������ң�����״̬��һ�¡�
		// ���ܴ���������������γ�ʼ�����ܣ���
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}
	


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

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)//CallbackTag�������˺������õ� GameplayTag���� State.HitReacting����NewCount���ñ�ǩ��ǰ�������������ж��Ƿ������˳��ܻ�״̬����
{
	bHitReacting = NewCount > 0;//��� NewCount > 0����ʾ��ǩ����ӣ����� bHitReacting Ϊ true�����������ܻ�������� NewCount == 0����ʾ��ǩ���Ƴ������� bHitReacting Ϊ false���ܻ���������

	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;//���Ƶ����ƶ��ٶȡ���� bHitReacting Ϊ true������ MaxWalkSpeed Ϊ 0���ܻ�ʱ����ֹͣ�ƶ�����

	if (AuraAIController && AuraAIController->GetBlackboardComponent())//��ȫ��飺��� AuraAIController�����˵� AI ���������Ƿ���ڡ����ÿ������� BlackboardComponent����Ϊ���ĺڰ壩�Ƿ���ڡ�
        /*Ϊʲô��Ҫ��
		�ͻ��˿���û�� AIController�����ڷ�������Ч����
		��Ϊ��δ���úڰ�ʱ��GetBlackboardComponent() ���� nullptr*/
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);//���ܻ�״̬ͬ������Ϊ���ĺڰ��С�
		                                                                                              //������"HitReacting"��������Ϊ���ж���ļ���һ�£���
																									  // ��ֵ��bHitReacting��true / false��
	}
}
	

void AAuraEnemy::InitAbilityActorInfo()
{
	//InitAbilityActorInfo() �������������ڳ�ʼ��ĳ����Ϸʵ�壨���ɫ����ߣ���������Ϣ�ġ�
    // ����ܰ������ø�ʵ��ļ��ܡ����ԡ���������������������ص����ݡ�
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	OnASCRegisteredDelegate.Broadcast(AbilitySystemComponent);
	
	
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::StunTagChanged);
	

	if (HasAuthority())
	{
		InitializeDefualtAttributes();
	}
	//InitializeDefaultAttributes �Ƿ�����Ȩ���߼���
	//��ʼ��Ĭ�����ԣ���Ѫ�����������������ɷ�������ɣ��ͻ���ֻ�����ͬ����Ľ����
	//����ͻ���Ҳ�ܳ�ʼ�����ԣ����ܵ�����ֵ���۸ģ�����ͻ����޸��Լ���Ѫ������

	//InitAbilityActorInfo ��������������Ȩ�޼�飺
    //AbilitySystemComponent->InitAbilityActorInfo(this, this) �� AbilityActorInfoSet() �ǻ������ã��ͻ��˺ͷ���������ִ�У������ASC��Actor��
}

void AAuraEnemy::InitializeDefualtAttributes() const
{
	//InitializeDefaultAttributesӦ���ڷ�������� ����Ӧ�ü��Ȩ�� HasAuthority()
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);

	if (AuraAIController&&AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
	}
}

int32 AAuraEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}
