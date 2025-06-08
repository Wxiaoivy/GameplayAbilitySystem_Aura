// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemBlueprintLibrary.h>
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayAbilitySpec.h>
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent()
{

}

//这个函数被AuraCharacterBase的同名函数调用
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	// 这是一个 for 循环，用于遍历 StartupAbilities 数组中的每一个能力类。
	// TSubclassOf<UGameplayAbility>AbilityClass: 每次循环时，AbilityClass 会代表数组中的一个能力类。
	// StartupAbilities: 是传入的能力数组。
	for (const TSubclassOf<UGameplayAbility>AbilityClass : StartupAbilities)
	{
		//FGameplayAbilitySpec: 是 Gameplay Ability System (GAS) 中用来描述能力实例的结构体。
		// AbilityClass: 当前循环中的能力类。
		// 1 : 表示能力的等级（Level），这里设置为 1。
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		//检查 AbilitySpec.Ability 是否是 UAuraGameplayAbility 类型的实例。
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast< UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//如果是，则将一个输入标签（StartupInputTag）添加到能力实例的 DynamicAbilityTags 中
			//AbilitySpec.DynamicAbilityTags: 是能力实例的动态标签集合，用于存储与能力相关的标签。
			//AuraGameplayAbility->StartupInputTag: 是 UAuraGameplayAbility 中定义的输入标签，表示该能力对应的输入（如按键）。
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			//将能力实例（AbilitySpec）添加到角色的 AbilitySystemComponent 中。
			GiveAbility(AbilitySpec);

			
			bStartupAbilitiesGiven = true;
			AbilitiesGivenDelegate.Broadcast();
		}
		                    

	   //将能力实例（AbilitySpec）添加到角色的 AbilitySystemComponent 中。
	   //解释:
	   //GiveAbility: 是 AbilitySystemComponent 的一个函数，用于将能力添加到角色中。
       //AbilitySpec : 是上一步创建的能力实例。
	   //GiveAbility(AbilitySpec);//用这个函数可以在上一行代码写const（区别）
		
	
	   //GiveAbilityAndActivateOnce: 是 AbilitySystemComponent 的一个函数，用于添加能力并立即激活。
		//GiveAbilityAndActivateOnce(AbilitySpec);//用这个函数不可以在上面第二行代码写const（区别）


		//GiveAbilityAndActivateOnce 会立即激活能力，如果不需要立即激活，可以只使用 GiveAbility。

		//如果能力需要特定的激活条件（如按键触发），则不需要调用 GiveAbilityAndActivateOnce。
	}
}
void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility>AbilityClass : StartupPassiveAbilities)
	{
		//FGameplayAbilitySpec: 是 Gameplay Ability System (GAS) 中用来描述能力实例的结构体。
		// AbilityClass: 当前循环中的能力类。
		// 1 : 表示能力的等级（Level），这里设置为 1。
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

//这个函数被AuraPlayerController的同名函数调用
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);

			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

//这个函数被AuraPlayerController的同名函数调用
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);

		}
	}

}

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//OnGameplayEffectAppliedDelegateToSelf是父类的一个代理实例，这里绑定回调函数到此代理
	//OnGameplayEffectAppliedDelegateToSelf每当一个游戏效果（Game Effect，简称GE）被应用到自身上时，无论是即时效果还是持续效果，服务器都会调用此方法。”
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

	//通过FAuraGameplayTags::Get()方法获得了该类的唯一单例储存在GameplayTags中
	//通过实例可以访问Attribute_Secondary_Armor成员变量
	//const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Orange, FString::Printf(TEXT("Tag:%s"), *GameplayTags.Attribute_Secondary_Armor.ToString()));
	// FGameplayTag Tag = FAuraGameplayTags::Get().Attribute_Primary_Intelligence;//测试可以在C++中访问这些native tags
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{

	// 定义一个游戏玩法标签容器（TagContainer），用于存储从效果规范中提取的标签。
	FGameplayTagContainer TagContainer;
	// 从效果规范中获取所有的资产标签，并将它们存储在TagContainer中。
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTag.Broadcast(TagContainer);//广播代理到WidgetController（发送数据）
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{

	FScopedAbilityListLock ActiveScopeLock(*this);//FScopedAbilityListLock 是 UE 提供的 RAII（资源获取即初始化）锁，用于 线程安全。防止在遍历过程中技能列表被修改（如技能被移除或添加）。
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))//无论是否进入 if 块，ExecuteIfBound 都会先尝试执行代理。if 只是根据执行结果决定是否进入分支，不影响代理本身的执行。
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFormSpec(const FGameplayAbilitySpec& AbilitySpec)
{

	if (AbilitySpec.Ability)
	{
		//从技能标签中提取属于 "Abilities" 分类的标签（如 "Abilities.Fireball"）。
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;// 返回匹配 "Abilities" 父标签的Tag
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFormSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	//从动态标签中提取输入绑定标签（如 "Input.Q"）
	for (FGameplayTag InputTag : AbilitySpec.DynamicAbilityTags)
	{
		if (InputTag.MatchesTag((FGameplayTag::RequestGameplayTag(FName("InputTag")))))
		{
			return InputTag;// 返回匹配 "Input" 父标签的Tag
		}
	}
	return FGameplayTag();
}


FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFormSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	//从动态标签中提取Status标签（如 "Abilities.Status.Lock"）
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag((FGameplayTag::RequestGameplayTag(FName("Abilities.Status")))))
		{
			return StatusTag;// 返回匹配 "Abilities.Status" 父标签的Tag
		}
	}
	return FGameplayTag();
}


FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);//创建一个作用域锁，确保在遍历技能列表时不会被其他线程修改  这是线程安全措施
	for (auto AbilitySpec:GetActivatableAbilities())//遍历所有可激活的技能GetActivatableAbilities() 返回的是当前 已经通过 GiveAbility() 
		                                            //赋予 给 AbilitySystemComponent (ASC) 并且 可以被激活 的技能列表。
	{
		for (auto Tag : AbilitySpec.Ability.Get()->AbilityTags)//遍历当前技能的标签列表(AbilityTags),每个技能可以有多个标签
		{
			if (Tag.MatchesTag(AbilityTag))//检查当前标签是否与传入的标签匹配
			{
				return &AbilitySpec;//如果找到匹配标签的技能，返回该技能的指针
			}
		}
	}
	return nullptr;//遍历完所有技能都没找到匹配的，返回空指针
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)//这个函数检查玩家的等级，如果满足条件（Level >= Info.LevelRequirement），
                                                                  //并且技能 还没有被赋予（GetSpecFromAbilityTag(Info.AbilityTag) == nullptr），才会调用 GiveAbility() 赋予新技能。
                                                                  //这样，新技能就会被添加到 GetActivatableAbilities() 的列表里，之后就可以激活它了。
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());//获取能力信息数据资产(DataAsset) GetAbilityInfo()是自定义的辅助函数，用于获取技能信息
	for (const auto Info : AbilityInfo->AbilityInformation)//遍历所有定义的技能信息,Info是包含技能定义的结构体
	{
		if (!Info.AbilityTag.IsValid())continue;//跳过无效标签的技能
		if(Level<Info.LevelRequirement)continue;//如果玩家等级不满足技能等级要求，跳过	
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)//检查玩家是否还没有这个技能(使用前面定义的GetSpecFromAbilityTag函数)
	    {
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);//创建新的技能规格,Info.Ability是技能类,1是技能等级
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);//给技能添加动态标签"Eligible"(合格/可用)
			GiveAbility(AbilitySpec);//将技能赋予玩家
			MarkAbilitySpecDirty(AbilitySpec);//标记技能规格为"脏"，确保同步(网络游戏中很重要),强制服务器复制给客户端
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible);//Client 表示这个函数只会在 服务器调用，
			                                                                                               //然后在 owning client（拥有这个组件的客户端）上执行。
			                                                                                               //服务器不会执行这个函数，它只是通过网络让客户端执行。
			                                                                                               //这种设计是典型的 "服务器授权，客户端表现" 模式，服务器控制逻辑，客户端负责更新 UI 或视觉效果。
	    }
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag);
}


void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)//bStartupAbilitiesGiven 是一个普通变量（非复制变量），因此：服务器设置为 true 不会自动同步到客户端。客户端需要在 自己的执行环境 中独立设置 bStartupAbilitiesGiven
	{
		//ActivateAbilities 被复制到客户端 → 触发 OnRep_ActivateAbilities。此时客户端的 bStartupAbilitiesGiven 仍是默认值 false（因为未同步）。客户端需要 主动检测 并标记 bStartupAbilitiesGiven = true
		bStartupAbilitiesGiven = true;//防御性检查 (if (!bStartupAbilitiesGiven)) 是多人游戏编程的黄金法则，确保逻辑健壮性。这种设计是虚幻引擎 GAS 的常见模式，适用于所有需要“首次数据就绪”通知的场景。
		AbilitiesGivenDelegate.Broadcast();
	}
}



void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			SeverUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::SeverUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}

}



