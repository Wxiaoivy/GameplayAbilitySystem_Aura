// Copyright Ivy


#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerController.h"
#include "Interaction/PlayerInterface.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();


	//在 C++ 中，函数名本身代表的是函数的地址（即函数指针），而函数名加括号（如 GetStrengthAttribute()）表示对函数的调用。
	// 因此，在 TagsToAttributes.Add 中，GetStrengthAttribute 不加括号是因为这里需要的是函数的地址（函数指针），而不是调用函数的结果。


	/*Primary Attributes*/
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Strength, GetStrengthAttribute);//这个GetStrengthAttribute是通过ACCESSORS访问的函数，是静态函数。
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Vigor, GetVigorAttribute);


	/*Secondary Attributes*/
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_MaxMana, GetMaxManaAttribute);


	/*Resistance Attributes*/
	TagsToAttributes.Add(GameplayTags.Attribute_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Resistance_Physical, GetPhysicalResistanceAttribute);


}




// UAuraAttributeSet类的GetLifetimeReplicatedProps函数实现
// 这个函数会调用父类的同名函数，并添加Health属性到需要复制的属性列表中
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 使用DOREPLIFETIME_CONDITION_NOTIFY宏来指定Health属性需要复制，并且总是触发OnRep_Health函数
	// COND_None表示没有特定的条件需要满足，
	// REPNOTIFY_Always表示总是触发通知

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
 
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);




}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange( Attribute, NewValue);
	

	//这里的NewValue是CurrentValue,周期性GE是修改BaseValue的,BaseValue被修改时会重新计算相关属性的聚合器中的各项Mod,间接修改CurrentValue 
	//只钳制了CurrentValue没有钳制BaseValue GetValue获取的是CurrentValue 所以实际上BaseValue一直在溢出
	/*if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}*/
}

void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	//这里的NewValue是BaseValue


	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}



//FGameplayEffectModCallbackData会包含以下关键信息：

//被修改的属性：指明哪个属性被Gameplay Effect所修改。
//修改前后的值：包含属性修改前后的基值（BaseValue）和当前值（CurrentValue），这有助于开发者了解Gameplay Effect对属性的具体影响。
//Gameplay Effect的引用：提供对触发此次修改的Gameplay Effect的引用，以便开发者在回调函数中根据需要获取更多关于该效果的信息。
void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)const
{
	// 从Data中获取游戏效果的上下文（Context），这个上下文包含了关于游戏效果如何被执行的信息。
	
	Props.EffectContextHandle = Data.EffectSpec.GetContext();

	// 从上下文中获取触发游戏效果的原始发起者的能力系统组件（ASC）。
	// ASC是管理角色能力、属性和游戏效果的核心组件。
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	// 检查SourceASC是否有效，以及它的AbilityActorInfo和AvatarActor是否都有效。
	// AbilityActorInfo包含了关于能力系统组件所关联角色的信息，AvatarActor是这个角色的实际游戏对象。
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		// 从AbilityActorInfo中获取角色的游戏对象（Actor）
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();

		// 尝试从AbilityActorInfo中获取角色的控制器（Controller）。
		// 控制器是处理玩家输入和角色交互的组件。
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		// 如果没有找到控制器，但找到了角色的游戏对象，那么尝试从游戏对象中获取控制器。
		// 这通常发生在角色是由AI控制而不是玩家控制的情况下。
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{

			// 如果SourceAvatarActor可以被转换为APawn类型（APawn是AActor的一个子类，代表可以被控制的角色），
			// 那么从它那里获取控制器。
			if (const APawn* Pawn = Cast <APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();

			}

		}

		// 如果找到了控制器，那么尝试从控制器中获取它控制的角色（Pawn），
		// 然后将这个Pawn转换为ACharacter类型（ACharacter是APawn的一个子类，代表具有特定游戏逻辑的角色）。
		// 注意：这里的转换可能失败，如果Pawn不是ACharacter类型的话，但代码中没有处理这种情况。
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	// 检查Data中的目标信息是否有效，以及目标的AvatarActor是否有效。
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		// 从目标的能力系统信息中获取目标的游戏对象（Actor）。
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();

		// 尝试从目标的能力系统信息中获取目标的控制器。
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();

		// 尝试将目标的游戏对象转换为ACharacter类型。
		// 注意：这里的转换可能失败，但代码中没有处理这种情况。
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);

		// 使用UAbilitySystemBlueprintLibrary的静态函数GetAbilitySystemComponent来获取目标的游戏对象的能力系统组件。
		// 这个函数是Unreal Engine中用于获取ASC的常用方法
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

// 这个函数在游戏效果执行后被调用，允许我们处理游戏效果执行后的逻辑。
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);


	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	//if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	//{
	//	SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	//}

	/*if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	*/
	//如果Target死亡了， 就跳过后续逻辑。
	if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter))return;
	
	//检查被修改的属性是否是"IncomingDamage"(传入伤害)
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	//Data.EvaluatedData.Attribute == GetIncomingXPAttribute() 表示：GAS 已经完成了对 IncomingXP 的修改，现在正在处理后续逻辑。GetIncomingXP() 读取的是 已经被 GameplayEffect 更新后的值。
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::HandleIncomingXP(FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();//获取当前的XP值

	SetIncomingXP(0);//立即将XP属性重置为0(为下一次到来的XP做准备)

	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 NumLevelUps = NewLevel - CurrentLevel;

		if (NumLevelUps > 0)
		{

			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);


			int32 AttributePointsReward = 0; 
			int32 SpellPointsReward = 0;
			for (int32 i = 0; i < NumLevelUps; i++)
			{
				AttributePointsReward += IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter,CurrentLevel+i);
				SpellPointsReward+= IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel+i);
			}

			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

			bTopOffHealth = true; //当角色升级时，bTop0ffHealth和bTop0ffMana被设置为true（在升级逻辑中设置）,但只有在MaxHealth或MaxMana属性发生变化时，才会进入PostAttributeChange函数
			bTopOffMana = true;

			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}

		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}



void UAuraAttributeSet::HandleIncomingDamage(FEffectProperties& Props)
{
	
	const float LocalIncomingDamage = GetIncomingDamage();//获取当前的伤害值

	SetIncomingDamage(0);//立即将伤害属性重置为0(为下一次伤害做准备)

	if (LocalIncomingDamage > 0)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;//计算新的生命值(当前生命值减去伤害)

		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));//使用FMath::Clamp确保生命值在0和最大值之间

		const bool bFetal = NewHealth <= 0;//检查新生命值是否小于等于0(即是否死亡)

		if (bFetal)
		{
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
			if (CombatInterface)
			{
				FVector DeathImpuse = UAuraAbilitySystemLibrary::GetDeathImpuse(Props.EffectContextHandle);
				CombatInterface->die(DeathImpuse);
			}
			SendXPEvent(Props);
		}
		else
		{
			//如果没有死亡，并且没有在被电击中，则尝试激活带有"Effects_HitReact"标签的技能
			//这会触发受击动画或其他受击反应逻辑
			if (Props.TargetCharacter->Implements<UCombatInterface>()&&!ICombatInterface::Execute_IsBeingShocked(Props.TargetCharacter))
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}

			//Data是引擎提供的原始回调数据，包含所有GameplayEffect执行的原始信息
			// Props是开发者自定义的结构体，通过SetEffectProperties函数从Data中提取并组织成更易用的形式

			// 典型情况下，Props可能包含：

			//目标ASC(AbilitySystemComponent)

			//来源ASC

			//目标Actor

			//来源Actor

			//效果规格(EffectSpec)等
			const FVector& KnockbackForce = UAuraAbilitySystemLibrary::GetKnockBackForce(Props.EffectContextHandle);
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}
		
		}
		bool bIsBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		bool bIsCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		ShowFloatingText(Props, LocalIncomingDamage, bIsBlockedHit, bIsCriticalHit);
		if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			/*动态创建的Debuff的EffectContext没有继承原始Context中的bIsSuccessfulDebuff标记，
			那么后续的周期性伤害虽然会触发PostGameplayEffectExecute，但是不会满足 IsSuccessfulDebuff 条件，
			从而不会触发二次Debuff。这种情况下，你的代码已经天然避免了循环触发，*/
			Debuff(Props);
		}
	
	}
}

void UAuraAttributeSet::ShowFloatingText(FEffectProperties& Props, const float LocalIncomingDamage, bool bIsBlockedHit, bool bIsCriticalHit)
{
	if ( Props.SourceCharacter && Props.TargetCharacter && Props.SourceCharacter != Props.TargetCharacter)
	{

		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceCharacter->GetController()))
		{
			//对于玩家是攻击者(SourceCharacter)：当玩家攻击别人时，可能想在攻击目标身上显示造成的伤害数字
			PC->ShowDamageNumber(LocalIncomingDamage, Props.TargetCharacter, bIsBlockedHit, bIsCriticalHit);
			return;
		}
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetCharacter->GetController()))
		{
			//对于玩家是受击者(TargetCharacter)：当玩家被攻击时，可能想在自己角色身上显示受到的伤害数字
			PC->ShowDamageNumber(LocalIncomingDamage, Props.TargetCharacter, bIsBlockedHit, bIsCriticalHit);
		}
	}
}

void UAuraAttributeSet::Debuff(FEffectProperties& Props)
{
	//关键是理解：原始伤害和后续Debuff属于不同的效果实例，需要独立但关联的上下文管理。
	/*必须这样做的原因
		上下文隔离：
		原始Context属于造成伤害的GameplayEffect
		Debuff需要自己的独立Context（可能持续很长时间）

		数据持久化：
		Debuff可能持续10秒，期间原始Context早已销毁
		必须将关键数据（DamageType）拷贝到新Context

		效果链追溯：
		保留伤害类型信息，便于后续：
		显示正确的Debuff图标
		计算抗性减免
		触发对应的GameplayCue*/

		/*数据需要经过：
		首次存储：在Execution阶段存入原始Context （在ExecCalc_Damage中）在 ExecCalc_Damage 中计算后存入原始Context，供 AttributeSet 使用，
		        （原始Context属于造成伤害的GameplayEffect，它储存了Debuff的数据，但是没使用，只是储存来供二次拷贝到新的Context）
				 原始Context（来自造成非Debuff伤害的GameplayEffect）确实会存储Debuff相关数据，但它的核心作用仅仅是 临时暂存这些数据，供后续在创建独立的Debuff效果时 拷贝到新的Context中。
		二次传递：创建持续效果时拷贝到新Context    （在这里）*/


	//1. 获取游戏标签和创建效果上下文
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();// 创建一个新的效果上下文（Context）
	EffectContext.AddSourceObject(Props.SourceAvatarActor);// 设置效果的来源对象（比如是哪个角色释放的这个Debuff）


	//2.从原始Context中读取Debuff参数
	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);// 从造成伤害的原始Context中获取Debuff类型（比如火焰、中毒）
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);// 读取Debuff的伤害值（比如每秒扣5点血）
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);// 读取Debuff的持续时间（比如持续10秒）
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);// 读取Debuff的触发频率（比如每1秒触发一次伤害）
	

	//3. 动态创建Debuff效果（GameplayEffect）
	//动态创建的Debuff的EffectContext没有继承原始Context中的bIsSuccessfulDebuff标记，
	// 那么后续的周期性伤害确实不会满足 IsSuccessfulDebuff 条件，从而不会触发二次Debuff。
	// 这种情况下，你的代码已经天然避免了循环触发，无需额外防护。
	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());// 给这个动态创建的Debuff起个名字（比如"DynamicDebuff_Fire"）
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));// 创建一个新的GameplayEffect对象（临时存在内存中，不被保存到磁盘）


	//4. 配置Debuff的基础属性
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;// 设置Debuff的持续时间类型为"有固定持续时间
	Effect->Period = DebuffFrequency;// 设置Debuff的触发间隔（比如每1秒触发一次）
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);// 设置Debuff的总持续时间（比如10秒)
	Effect->bExecutePeriodicEffectOnApplication = false; // 禁用首次立即触发


	//5. 添加标签和堆叠规则
	//为动态创建的 Debuff 打上类型标签，实现 逻辑分类 和 行为触发。关键优势：通过标签系统将 Debuff 类型与游戏逻辑解耦，提升可维护性和扩展性。
    const FGameplayTag DebuffTag = GameplayTags.DamageTypesToDebuff[DamageType];
	Effect->InheritableOwnedTagsContainer.AddTag(DebuffTag);// 告诉游戏这是一个“火焰Debuff”，用于触发通用逻辑（这个是标识作用 ，不会把Tag添加到ASC里面，而在449行才把Tag添加到ASC接着触发AbilitySystemComponent->RegisterGameplayTagEvent
	if (DebuffTag.MatchesTagExact(GameplayTags.Debuff_Stun))
	{
		Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.Player_Block_CursorTrace);
		Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.Player_Block_InputHeld);
		Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.Player_Block_InputPresssed);
		Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.Player_Block_InputReleased);

	}
	

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;    //1. AggregateBySource 的核心定义
	Effect->StackLimitCount = 1.f;                                            /*作用：
	// 设置同一个来源的Debuff最多叠加1层（防止同一个敌人反复触发导致无限叠加）        当多个相同类型的 GameplayEffect 由 不同的来源（Source） 应用时，GAS 会根据来源区分并独立管理它们的堆叠。
																				关键特性：
																				每个 独立的来源 可以拥有一个该效果的实例。
																				同来源的重复应用会被合并（受 StackLimitCount 限制）。
																				不同来源的效果会共存，各自独立计算持续时间和效果。*/
	
	
	//6. 配置Debuff的数值修改器
	//每次调用 Debuff() 函数：都会执行 NewObject<UGameplayEffect>()，生成一个全新的独立实例。Modifiers 数组初始为空：因此每次都需要重新走 Add() 流程。
	const int32 index = Effect->Modifiers.Num();//获取当前 GameplayEffect 中已有的修改器数量。
	Effect->Modifiers.Add(FGameplayModifierInfo());//向 Modifiers 数组末尾添加一个新的默认修改器。Add() 会扩展数组大小,
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[index];	//为什么这样写：先获取当前数量（Num()），确保插入位置正确。添加后立即通过索引获取引用，避免后续反复查询数组。
	                                                                //每次触发新的Debuff时，确实会重新创建一个全新的 GameplayEffect 实例，其 Modifiers 数组会经历从 0 → 1 的添加过程。
																	//第一步：index 记录当前数组长度（此时为 0）。
                                                                    //第二步：向数组添加一个默认构造的修改器（FGameplayModifierInfo()），数组变为[空修改器]，长度变为 1。
		                                                            //第三步：通过之前记录的 index（值为 0）获取这个新添加的修改器的引用。(数组第一个元素Index为0，跟最先的Num一样）
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = GetIncomingDamageAttribute();


	//7. 应用Debuff到目标
	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))// 创建一个效果规格（Spec），包含效果的具体参数和上下文
	{
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());// 获取自定义的上下文，并存储伤害类型（用于后续抗性计算等）
		TSharedPtr<FGameplayTag>DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AuraContext->SetDamageType(DebuffDamageType);// 保留精确的伤害类型（Damage_Fire），
		                                             //用于数值计算(Effect->InheritableOwnedTagsContainer.AddTag("Debuff.Fire"作用不同);
		MutableSpec->DynamicGrantedTags.AddTag(DebuffTag);//AI让我自己加的 作用：向一个 GameplayEffectSpec 动态添加一个游戏标签（如Debuff_Burn），
		                                                                             //这个标签会在效果被应用时附加到目标身上。
		                                                                             /*DynamicGrantedTags 里的标签会 直接附加到目标（Target）的 AbilitySystemComponent(ASC) 上。
			                                                                          也就是说，目标会临时获得 Debuff_Burn 标签，直到效果结束。*/// 关键修复：必须添加 DynamicGrantedTags


		//添加BlockTag（用于眩晕时禁止鼠标的输入，键盘没禁止 ）
		//方法一（推荐）
		if (DebuffTag.MatchesTagExact(GameplayTags.Debuff_Stun))
		{
			MutableSpec->DynamicGrantedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
			MutableSpec->DynamicGrantedTags.AddTag(GameplayTags.Player_Block_InputHeld);
			MutableSpec->DynamicGrantedTags.AddTag(GameplayTags.Player_Block_InputPresssed);
			MutableSpec->DynamicGrantedTags.AddTag(GameplayTags.Player_Block_InputReleased);

		}

		//方法二（需要手动Remove)
	/*	if (UAuraAbilitySystemComponent*AuraASC=Cast<UAuraAbilitySystemComponent>(Props.TargetASC))
			{
				FGameplayTagContainer BlockedTags;
				BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
				BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
				BlockedTags.AddTag(GameplayTags.Player_Block_InputPresssed);
				BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
				
				if (DebuffTag.MatchesTagExact(GameplayTags.Debuff_Stun))
				{
					AuraASC->AddLooseGameplayTags(BlockedTags);
				}
				else
				{
					AuraASC->RemoveLooseGameplayTags(BlockedTags);
				}
			}*/
		

		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);// 将Debuff应用到目标身上（触发周期性的属性修改）

	}
	//初始伤害->触发Debuff->动态创建新GE->添加Modifier到空数组->应用GE到目标->持续时间结束->GE自动销毁
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	//在函数内部，当检测到对应的属性变化且布尔值为true时，才会调用SetHealth()或SetMana()来补满数值
	//这意味着如果升级时MaxHealth/MaxMana没有实际变化（比如最大值没有提升），PostAttributeChange不会被调用，血/蓝也就不会被补满。	 
	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
    {
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
    }
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	
	if (Props.TargetCharacter->Implements<UCombatInterface>())//检查目标角色(Props.TargetCharacter)是否实现了ICombatInterface接口,ICombatInterface里面有2个函数可以获得等级和职业
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);//获取目标的等级(TargetLevel)和职业(TargetClass)这些信息将用于计算XPReward
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);//获取目标的等级(TargetLevel)和职业(TargetClass)这些信息将用于计算XPReward
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);//调用一个静态函数GetXPRewardForClassAndLevel来计算经验值

		//创建一个FGameplayEventData结构体Payload(事件数据)设置事件标签为"Attribute_Meta_IncomingXP"(表示"获得经验值"事件)设置事件数值(EventMagnitude)为计算出的经验值奖励
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags().Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attribute_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;

		//这个Payload里的数据传递给蓝图GA_ListenForEvent执行后续SetByCallerMagenitude和ApplyEffectSpecToSelf的逻辑
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attribute_Meta_IncomingXP, Payload);
		//将创建的事件发送给源角色(Props.SourceCharacter，即击杀者)这个事件会被角色的AbilitySystemComponent接收并处理
	}
}


// UAuraAttributeSet类的OnRep_Health函数实现
// 这个函数会在Health属性在网络上被复制时被调用
// GAMEPLAYATTRIBUTE_REPNOTIFY宏用于处理属性复制的通知逻辑
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//GAMEPLAYATTRIBUTE_REPNOTIFY这个宏专门用于处理游戏玩法属性（Gameplay Attributes）复制时的通知
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}