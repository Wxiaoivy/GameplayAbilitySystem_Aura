// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemBlueprintLibrary.h>
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayAbilitySpec.h>
#include <../../../../../../../Source/Runtime/Engine/Public/Net/UnrealNetwork.h>
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent()
{

}

//FGameplayEffectContextHandle UAuraAbilitySystemComponent::MakeEffectContext() const
//{
//	// 返回自定义的 FAuraGameplayEffectContext
//	return FGameplayEffectContextHandle(new FAuraGameplayEffectContext());
//}

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
//这个函数被AuraPlayerController的同名函数调用(每次单击时只调用一次能力输入标记按下)
void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)//AbilityInputTagPressed	按键按下瞬间	仅同步事件到服务器（不激活能力）
{
	//用途：
	//	支持 WaitInputPress 节点（服务器需要知道“按下”事件）。
	//	不涉及实际的能力激活（避免短按误触发）。
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.IsActive())
		{           
			// 关键：通知服务器输入释放事件（确保权威同步）
			// 用途：支持蓝图节点如 `WaitInputPressed` 的服务器端检测
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}


//3. 典型应用场景
//案例1：蓄力技能
//玩家按下鼠标右键（Pressed）：
//仅通知服务器“右键按下”，但不激活技能。
//
//玩家按住右键（Held）：
//匹配标签后调用 AbilitySpecInputPressed，开始蓄力。
//
//玩家释放右键（Released）：
//根据蓄力时间触发技能。
//
//案例2：瞬发技能
//通过 Held 直接激活（即使按住时间很短），但标签匹配避免了误触其他能力。


//这个函数被AuraPlayerController的同名函数调用(调用时机：在按键按住期间，每帧（Tick）都会被调用，直到按键释放。)
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)//负责 实际的能力激活 和输入状态更新（通过 AbilitySpecInputPressed）。
{
	/*首次按下按键时，调用顺序为：
		Pressed → Held（同一帧）
		之后如果继续按住，每帧都会调用 Held。

	典型用途：
		激活瞬发技能（通过 TryActivateAbility 立即触发）。
		处理蓄力 / 持续技能（通过每帧更新状态，如蓄力进度条）。*/
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);// 更新输入状态（即使瞬发技能也需要）
			// 关键点：如果技能未激活，就尝试激活
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);// 瞬发技能在此处立刻激活
			}
		}
		//瞬发技能：TryActivateAbility 会立即执行 ActivateAbility，技能表现和 Pressed 无异。

		//蓄力 / 持续技能：IsActive() 会返回 true，所以不会重复激活，而是进入 Held 逻辑（如蓄力进度更新）。
	}
}

//这个函数被AuraPlayerController的同名函数调用
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec); //本地立即响应（通过 AbilitySpecInputReleased）→ 保证操作的即时性（减少输入延迟）
			// 关键：通知服务器输入释放事件（确保权威同步）
			// 用途：支持蓝图节点如 `WaitInputRelease` 的服务器端检测
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
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
	for (FGameplayAbilitySpec& AbilitySpec:GetActivatableAbilities())//遍历所有可激活的技能GetActivatableAbilities() 返回的是当前 已经通过 GiveAbility() 
		                                            //赋予 给 AbilitySystemComponent (ASC) 并且 可以被激活 的技能列表。
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)//遍历当前技能的标签列表(AbilityTags),每个技能可以有多个标签
		{
			if (Tag.MatchesTag(AbilityTag))//检查当前标签是否与传入的标签匹配
			{
				return &AbilitySpec;//如果找到匹配标签的技能，返回该技能的指针
			}
		}
	}
	return nullptr;//遍历完所有技能都没找到匹配的，返回空指针
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusTagFormSpec(*Spec);
	}
	return FGameplayTag();
}


FGameplayTag UAuraAbilitySystemComponent::GetSlotTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFormSpec(*Spec);
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&AbilitySpec,Slot))
		{
			return false;
		}
	}
	return true;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFormSpec(AbilitySpec);
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	const FGameplayTag AbilityType = Info.AbilityType;
	return AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec)
{
	return AbilitySpec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot)
{
	ClearSlot(&AbilitySpec);
	AbilitySpec.DynamicAbilityTags.AddTag(Slot);
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, bool bActivate)
{
	ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)//这个函数检查玩家的等级，如果满足条件（Level >= Info.LevelRequirement），
                                                                  //并且技能 还没有被赋予（GetSpecFromAbilityTag(Info.AbilityTag) == nullptr），才会调用 GiveAbility() 赋予新技能。
                                                                  //这样，新技能就会被添加到 GetActivatableAbilities() 的列表里，之后就可以激活它了。
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());//获取能力信息数据资产(DataAsset) GetAbilityInfo()是自定义的辅助函数，用于获取技能信息
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)//遍历所有定义的技能信息,Info是包含技能定义的结构体
	{
		if (!Info.AbilityTag.IsValid())continue;//跳过无效标签的技能
		if(Level<Info.LevelRequirement)continue;//如果玩家等级不满足技能等级要求，跳过	
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)//检查玩家是否还没有这个技能(使用前面定义的GetSpecFromAbilityTag函数)
	    {
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);//创建新的技能规格,Info.Ability是技能类,1是技能等级
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);//给技能添加动态标签"Eligible"(合格/可用)
			GiveAbility(AbilitySpec);//将技能赋予玩家
			MarkAbilitySpecDirty(AbilitySpec);//标记技能规格为"脏"，确保同步(网络游戏中很重要),强制服务器复制给客户端
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible,1);//Client 表示这个函数只会在 服务器调用，
			                                                                                                                 //然后在 owning client（拥有这个组件的客户端）上执行。
			                                                                                                                 //服务器不会执行这个函数，它只是通过网络让客户端执行。
			                                                                                                                 //这种设计是典型的 "服务器授权，客户端表现" 模式，服务器控制逻辑，客户端负责更新 UI 或视觉效果。
	    }
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::SeverSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))//通过能力标签查找对应的FGameplayAbilitySpec,AbilitySpec包含能力的运行时信息（等级、标签等）,如果找不到对应规格，直接返回
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			/*GetAvatarActor()：获取拥有这个能力系统的角色
				检查角色是否实现了UPlayerInterface接口
				通过接口调用AddToSpellPoints，参数为 - 1表示扣除1点
				这是典型的Unreal接口调用方式*/
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();//获取游戏标签单例
		FGameplayTag StatusTag = GetStatusTagFormSpec(*AbilitySpec);//从能力规格中获取当前状态标签
		if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))//如果能力当前是"符合条件"状态：	
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);//移除"Eligible"标签
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);//添加"Unlocked"标签
			StatusTag = GameplayTags.Abilities_Status_Unlocked;//更新本地状态标签变量

			//这表示花费点数将符合条件的能力解锁
		}
		else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped)|| StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))//处理"已装备"或"已解锁"状态的能力,如果能力已经是"已装备"或"已解锁"状态：
		{
			AbilitySpec->Level += 1;// 直接提升能力等级 这表示花费点数用于升级已有能力
		}
		ClientUpdateAbilityStatus(AbilityTag, StatusTag, AbilitySpec->Level);//调用客户端RPC通知状态更新 参数包括：能力标签,新状态标签,新等级.  确保客户端与服务器同步
		MarkAbilitySpecDirty(*AbilitySpec);//通知能力系统该规格已修改,对于网络同步很重要，确保变更被复制
	}
	/*状态转换逻辑
		这个函数实现了以下状态转换逻辑：

		从符合条件到已解锁：
		当花费点数在"Eligible"状态时
		状态变化：Eligible → Unlocked
		典型应用：解锁新能力


		能力升级：
		当花费点数在"Unlocked"或"Equipped"状态时
		直接增加能力等级
		典型应用：强化已有能力


		网络同步流程:

		客户端发起花费点数请求
		服务器执行此函数：
		扣除点数
		修改能力状态
		通过RPC通知客户端
		客户端收到更新后同步UI

		设计要点:

		服务器权威：
		所有关键操作都在服务器执行
		客户端只能通过RPC请求

		状态驱动：
		根据当前状态决定不同行为
		使用GameplayTag灵活表示状态

		网络效率：
		只同步必要的变更
		使用MarkDirty优化同步

		扩展性：
		通过接口与玩家状态交互
		容易添加新的状态处理*/
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

bool UAuraAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AuraGameplayAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraGameplayAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid()|| AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))//这里因为升级时SpellPoint产生变化 触发OnSpellPointsChangedDelegate代理事件，
		                                                                                            //GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		                                                                                            //SelectedAbility里面的Ability初始化的默认值是Abilities_None，
																									//所以这里就算没有选择法术球，这个AbilityTag也是Abilities_None,所以会在升级时莫名其妙有Description
		                                                                                            //所以用这个if来避免
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}

	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::SeverEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag& PrevSlot = GetInputTagFormSpec(*AbilitySpec);//PrevSlot：技能之前绑定的槽位（如果没有绑定，可能是空标签）。
		const FGameplayTag& Status = GetStatusTagFormSpec(*AbilitySpec);//Status：技能当前的状态（Equipped 已装备 / Unlocked 可装备）。


		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;//只有 Equipped（已装备）或 Unlocked（可装备） 的技能才能被重新绑定。
		if (bStatusValid)
		{

			if (!SlotIsEmpty(Slot))
			{
				FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);
				if (SpecWithSlot)
				{
					if (AbilityTag.MatchesTagExact(GetAbilityTagFormSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, Status, Slot, PrevSlot);//ClientEquipAbility 是一个 客户端 RPC，通知客户端更新 UI（如技能栏图标）。
						return;
					}
					if (IsPassiveAbility(*SpecWithSlot))
					{
						MulticastActivatePassiveEffect(GetAbilityTagFormSpec(*SpecWithSlot), false);
						DeactivatePassiveAbility.Broadcast(GetAbilityTagFormSpec(*SpecWithSlot));
					}
					ClearSlot(SpecWithSlot);
				}
			}
			if (!AbilityHasAnySlot(*AbilitySpec))
			{
				if (IsPassiveAbility(*AbilitySpec))
				{
					MulticastActivatePassiveEffect(AbilityTag, true);
					TryActivateAbility(AbilitySpec->Handle);
				}
			}
			AssignSlotToAbility(*AbilitySpec, Slot);

			MarkAbilitySpecDirty(*AbilitySpec);//确保服务器和客户端同步技能数据。

			ClientEquipAbility(AbilityTag, Status, Slot, PrevSlot);//ClientEquipAbility 是一个 客户端 RPC，通知客户端更新 UI（如技能栏图标）。
		}
	}
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)//能力携带的InputTag
{
	// 1. 获取该技能当前绑定的输入槽（如 "Input.LMB"）
	const FGameplayTag Slot = GetInputTagFormSpec(*Spec);
	// 2. 从 DynamicAbilityTags 中移除这个槽位 Tag
	Spec->DynamicAbilityTags.RemoveTag(Slot);
}

void UAuraAbilitySystemComponent::ClearAbilityOfSlot(const FGameplayTag& Slot)
{
	// 1. 锁定技能列表（防止遍历时被修改）
	FScopedAbilityListLock ActiveScopeLock(*this);
	// 2. 遍历所有可激活的技能
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		// 3. 如果技能绑定了目标槽位（Slot），则清除绑定
		if (AbilityHasSlot(&Spec,Slot))
		{
			ClearSlot(&Spec);
		}
	}

	/*4. 整体流程示例
		假设玩家将 火球术 从 鼠标左键（Input.LMB） 解绑：
		调用 ClearAbilityOfSlot(Input.LMB)：
		遍历所有技能，找到绑定 Input.LMB 的技能（如火球术）。
		调用 ClearSlot 移除 Input.LMB 的 Tag。

		ClearSlot 内部：
		从 DynamicAbilityTags 中移除 Input.LMB。
		标记技能为“脏数据”，同步到客户端。

		结果：
		按下鼠标左键不再触发火球术。*/
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	// 遍历技能的所有动态 Tag
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		// 如果找到完全匹配的槽位 Tag，返回 true
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;// 未找到匹配的 Tag
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

