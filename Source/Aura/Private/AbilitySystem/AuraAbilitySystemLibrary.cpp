// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "UI/HUD/AuraHUD.h"

//返回一个布尔值(true/false)

//接受三个参数：
//WorldContextObject - 用于获取游戏世界的上下文
//OutWCParams - 输出的控件参数(引用传递，可以直接修改)
//OutAuraHUD - 输出的HUD对象(引用传递)
bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))//尝试获取玩家控制器(PlayerController)
	{
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD()); //从玩家控制器获取HUD(PC->GetHUD())并尝试转换为AAuraHUD类型
		if (OutAuraHUD)//检查是否成功获取了AuraHUD
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();//从玩家控制器获取玩家状态(PlayerState)并转换为AAuraPlayerState类型
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();//从玩家状态获取能力系统组件(Ability System Component)
			UAttributeSet* AS = PS->GetAttributeSet(); //从玩家状态获取属性集(Attribute Set)

			//将获取到的各个组件和对象赋值给输出参数OutWCParams的相应字段
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.AttributeSet = AS;
			OutWCParams.PlayerController = PC;
			OutWCParams.PlayerState = PS;

			return true;//如果一切顺利，返回true表示成功
		}
	}
	return false;//如果任何一步失败，最终返回false
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;//声明两个局部变量：WCParams - 控件参数结构体   AuraHUD - HUD指针，初始化为nullptr
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))//调用第一个函数MakeWidgetControllerParams来填充参数,如果返回true(成功)，进入if块
	{
		return AuraHUD->GetOverlayWidgetController(WCParams);//调用AuraHUD的GetOverlayWidgetController方法，传入准备好的参数,返回获取到的覆盖控件
	}
	return nullptr;//如果失败，返回空指针

	//上面这部分和下面注销掉的代码实现一个功能，功能是一样的。

	//if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	//{
	//	if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
	//	{
	//		AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
	//		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	//		UAttributeSet* AS = PS->GetAttributeSet();
	//		const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	//		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);

	//	}
	//}
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{

	FWidgetControllerParams WCParams;//声明两个局部变量：WCParams - 控件参数结构体   AuraHUD - HUD指针，初始化为nullptr
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))//调用第一个函数MakeWidgetControllerParams来填充参数,如果返回true(成功)，进入if块
	{
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);//调用AuraHUD的GetOverlayWidgetController方法，传入准备好的参数,返回获取到的覆盖控件
	}
	return nullptr;//如果失败，返回空指针

}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;//声明两个局部变量：WCParams - 控件参数结构体   AuraHUD - HUD指针，初始化为nullptr
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))//调用第一个函数MakeWidgetControllerParams来填充参数,如果返回true(成功)，进入if块
	{
		return AuraHUD->GetSpellMenuWidgetController(WCParams);//调用AuraHUD的GetOverlayWidgetController方法，传入准备好的参数,返回获取到的覆盖控件
	}
	return nullptr;//如果失败，返回空指针
}

//InitializeDefaultAttributes应该在服务器完成 所以应该检查权威 HasAuthority()
const void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	AActor* AvatarActor = ASC->GetAvatarActor();

	FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassDefaultInfo.PrimaryAttribute, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());


	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttribute, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttribute, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());




}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr)return;

	for (auto AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		//FGameplayAbilitySpec: 是 Gameplay Ability System (GAS) 中用来描述能力实例的结构体。
		// AbilityClass: 当前循环中的能力类。
		// 1 : 表示能力的等级（Level），这里设置为 1。(CommonAbilities因为是死亡，受击等能力不受等级变化而变化 ，所以默认就写1）
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	for (auto AbilityClass : DefaultInfo.StartupAbilities)
	{
		 if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		 {
			 FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			 ASC->GiveAbility(AbilitySpec);
		 }
	}
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr)return 0;

	const FCharacterClassDefaultInfo Info = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}


TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpaceRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator>Rotators;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	
	const float DeltaSpread = Spread / (NumRotators - 1);
	if (NumRotators > 1)
	{
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlRotateVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector>Vectors;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);

	const float DeltaSpread = Spread / (NumVectors - 1);
	if (NumVectors > 1)
	{
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	//GameMode只在服务器有效 如果是在客户端访问会返回Null,所以这里在客户端访问就会导致CharacterClassInfo也是Null
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameModeBase == nullptr)return nullptr;

	return AuraGameModeBase->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameModeBase == nullptr)return nullptr;

	return AuraGameModeBase->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->IsBlockHit();
	}

	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->IsCriticalHit();
	}

	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->IsSuccessfulDebuff();
	}

	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetDebuffDamage();
	}

	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetDebuffDuration();
	}

	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetDebuffFrequency();
	}

	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AuraContext->GetDamageType().IsValid())
		{
			return *AuraContext->GetDamageType();
		}
	}

	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpuse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetDeathImpuse();
	}

	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockBackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetKnockBackForce();
	}

	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool InIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		 AuraContext->SetIsBlockedHit(InIsBlockedHit);
	}

}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool InIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetIsCriticalHit(InIsCriticalHit);
	}

}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool InIsSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetIsSuccessfulDebuff(InIsSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDuration)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffFrequency)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpuse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpuse)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetDeathImpuse(InImpuse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockBackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockBackForce)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetKnockBackForce(InKnockBackForce);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
	const UObject* WorldContextObject,  // 用于获取World上下文的对象
	TArray<AActor*>& OutOverlapingActors,  // 输出参数：存储符合条件的Actor数组
	const TArray<AActor*>& ActorsToIgnore,  // 需要忽略的Actor列表
	float Radius,  // 搜索半径
	const FVector& SphereOrigin)  // 球体检测的原点位置
{
	FCollisionQueryParams SphereParams;//FCollisionQueryParams SphereParams 主要用于 配置碰撞检测的精细行为
	SphereParams.AddIgnoredActors(ActorsToIgnore);//SphereParams的核心作用：AddIgnoredActors  过滤不需要检测的Actor

	if (const UWorld*World=GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))//通过传入的WorldContextObject获取当前的World对象。如果获取失败则返回nullptr。
	{
		TArray<FOverlapResult>Overlaps;//创建用于存储重叠检测结果的数组。

		World->OverlapMultiByObjectType(//执行球体重叠检测，检测指定半径范围内所有与球体重叠的对象。
			Overlaps,  // 存储检测结果的数组
			SphereOrigin,  // 球体中心位置
			FQuat::Identity,  // 球体旋转(使用无旋转)
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),  // 检测所有动态对象
			FCollisionShape::MakeSphere(Radius),  // 创建球体碰撞形状
			SphereParams);  // 碰撞查询参数


	    for (FOverlapResult&Overlap: Overlaps)//遍历所有重叠检测结果。
	    {
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))//如果Actor实现了战斗接口并且是存活的：
				//短路求值是 && 和 || 的默认行为，主要用途：避免错误（如空指针、未实现接口的调用）。优化性能（跳过不必要的计算）如果 A 为false，整个表达式一定是 false，B 不会被执行。只有 A 为 true 时，才会计算 B。
			{	
					OutOverlapingActors.AddUnique(Overlap.GetActor());//将该Actor添加到输出数组中(确保不重复添加)
			}
	    }
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool BothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool BothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	if (BothArePlayers|| BothAreEnemies)
	{
		return false;
	}
	else
	{
		return true;
	}
}

// 定义一个静态函数，用于应用伤害效果
FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	// 获取游戏标签单例
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	// 从DamageEffectParams获取Avatar角色
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	// 创建效果上下文句柄
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	// 添加上下文源对象(施法者)
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	// 设置死亡冲击力到上下文
	SetDeathImpuse(EffectContextHandle, DamageEffectParams.DeathImpuse);
	// 设置击退力到上下文
	SetKnockBackForce(EffectContextHandle, DamageEffectParams.KnockBackForce);
	// 创建游戏效果规格句柄，包含效果类、能力等级和上下文
	FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->
	MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	// 设置基础伤害值(使用伤害类型作为标签)/设置Debuff触发几率/设置Debuff伤害值/ 设置Debuff触发频率/ 设置Debuff持续时间
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);

	// 将游戏效果应用到目标能力系统组件
	if (DamageEffectParams.TargetAbilitySystemComponent)
	{
		DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
	
	// 返回效果上下文句柄
	return EffectContextHandle;

	/*关键点说明：

		这个函数负责将配置好的伤害参数实际应用到目标上
		使用了SetByCaller方式动态设置各种伤害参数值
		通过游戏标签(GameplayTags)来标识不同类型的参数
		包含了死亡冲击力和击退力的设置
		最后将效果应用到目标的能力系统组件上
		返回的效果上下文句柄可以用于后续的效果追踪或处理*/
}

