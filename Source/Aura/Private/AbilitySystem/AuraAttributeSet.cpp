// Copyright Ivy


#include "AbilitySystem/AuraAttributeSet.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();


	//在 C++ 中，函数名本身代表的是函数的地址（即函数指针），而函数名加括号（如 GetStrengthAttribute()）表示对函数的调用。
	// 因此，在 TagsToAttributes.Add 中，GetStrengthAttribute 不加括号是因为这里需要的是函数的地址（函数指针），而不是调用函数的结果。
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Strength, GetStrengthAttribute);//这个GetStrengthAttribute是通过ACCESSORS访问的函数，是静态函数。
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Vigor, GetVigorAttribute);

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
			Props.SourceCharacter = Cast< ACharacter>(Props.SourceController->GetPawn());
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

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		//SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s,Health=%f"),*Props.TargetAvatarActor->GetName(),GetHealth())
	}

	/*if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	*/
}

// UAuraAttributeSet类的OnRep_Health函数实现
// 这个函数会在Health属性在网络上被复制时被调用
// GAMEPLAYATTRIBUTE_REPNOTIFY宏用于处理属性复制的通知逻辑
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//GAMEPLAYATTRIBUTE_REPNOTIFY这个宏专门用于处理游戏玩法属性（Gameplay Attributes）复制时的通知。
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
