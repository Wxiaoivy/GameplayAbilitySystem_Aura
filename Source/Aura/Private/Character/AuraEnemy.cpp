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
	if (HasAuthority())
	{ 
		//GiveStartupAbilities 是服务器权威逻辑：
		//BeginPlay 在客户端和服务器都会调用，但技能数据只需服务器初始化一次，然后通过网络同步

		//如果不加 HasAuthority() 的风险:
		//客户端可能会尝试给自己赋予技能，但无法同步到其他玩家，导致状态不一致。
		// 可能触发冗余操作（如多次初始化技能）。
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);
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

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed ? 0.f : BaseWalkSpeed;
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

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}
