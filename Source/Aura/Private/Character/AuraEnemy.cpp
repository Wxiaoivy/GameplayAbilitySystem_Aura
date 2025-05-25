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

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}



//PossessedBy()是角色被控制器接管时自动触发；（角色出生，重生，控制器变更 都会触发）
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

	GetCharacterMovement()->bUseControllerDesiredRotation = true;//需要敌人平滑转向
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AAuraEnemy::die()
{
	Super::die();
	if (AuraAIController)
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}
	
	SetLifeSpan(LifeSpan);
}



void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	if (HasAuthority())
	{ 
		//GiveStartupAbilities 是服务器权威逻辑：
		//BeginPlay 在客户端和服务器都会调用，但技能数据只需服务器初始化一次，然后通过网络同步

		//如果不加 HasAuthority() 的风险:
		//客户端可能会尝试给自己赋予技能，但无法同步到其他玩家，导致状态不一致。
		// 可能触发冗余操作（如多次初始化技能）。
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

		//RegisterGameplayTagEvent 是 ASC 提供的方法，用于 注册监听某个 Gameplay Tag 的变化。
	    //NewOrRemoved:当该 Tag 被 添加（Added） 或 移除（Removed） 时，会触发回调。
		//种方式在 GAS 中非常常见，用于 动态响应游戏状态变化，如受击、Buff/Debuff、技能触发等。
		AbilitySystemComponent->RegisterGameplayTagEvent
		(
			FAuraGameplayTags::Get().Effects_HitReact, // 监听的 Gameplay Tag
			EGameplayTagEventType::NewOrRemoved  // 监听类型（新增或移除）
		)
			.AddUObject
		(
			this,                                    // 回调对象（this）
			&AAuraEnemy::HitReactTagChanged          // 回调函数
		);

		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
	}
	 
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)//CallbackTag：触发此函数调用的 GameplayTag（如 State.HitReacting）。NewCount：该标签当前的数量（用于判断是否进入或退出受击状态）。
{
	bHitReacting = NewCount > 0;//如果 NewCount > 0，表示标签被添加，设置 bHitReacting 为 true（敌人正在受击）。如果 NewCount == 0，表示标签被移除，设置 bHitReacting 为 false（受击结束）。

	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;//控制敌人移动速度。如果 bHitReacting 为 true，设置 MaxWalkSpeed 为 0（受击时敌人停止移动）。

	if (AuraAIController && AuraAIController->GetBlackboardComponent())//安全检查：检查 AuraAIController（敌人的 AI 控制器）是否存在。检查该控制器的 BlackboardComponent（行为树的黑板）是否存在。
        /*为什么需要：
		客户端可能没有 AIController（仅在服务器有效）。
		行为树未配置黑板时，GetBlackboardComponent() 返回 nullptr*/
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);//将受击状态同步到行为树的黑板中。
		                                                                                              //键名："HitReacting"（需与行为树中定义的键名一致）。
																									  // 键值：bHitReacting（true / false）
	}
}
	

void AAuraEnemy::InitAbilityActorInfo()
{
	//InitAbilityActorInfo() 函数可能是用于初始化某个游戏实体（如角色或道具）的能力信息的。
    // 这可能包括设置该实体的技能、属性、动画或其他与其能力相关的数据。
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	if (HasAuthority())
	{
		InitializeDefualtAttributes();
	}
	//InitializeDefaultAttributes 是服务器权威逻辑：
	//初始化默认属性（如血量、攻击力）必须由服务器完成，客户端只需接收同步后的结果。
	//如果客户端也能初始化属性，可能导致数值被篡改（例如客户端修改自己的血量）。

	//InitAbilityActorInfo 的其他部分无需权限检查：
    //AbilitySystemComponent->InitAbilityActorInfo(this, this) 和 AbilityActorInfoSet() 是基础设置，客户端和服务器均需执行（例如绑定ASC到Actor。
}

void AAuraEnemy::InitializeDefualtAttributes() const
{
	//InitializeDefaultAttributes应该在服务器完成 所以应该检查权威 HasAuthority()
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

int32 AAuraEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}
