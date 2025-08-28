// Copyright Ivy


#include "Character/AuraCharacter.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/SpringArmComponent.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h>
#include "AuraGameplayTags.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Player/AuraPlayerController.h"
#include "Game/LoadScreenSaveGame.h"
#include "AbilitySystem/AuraAttributeSet.h"


AAuraCharacter::AAuraCharacter()
{
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;


	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;


	CharacterClass = ECharacterClass::Elementalist;

}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//Init Ability Actor Info for the Server
	InitAbilityActorInfo();
	LoadProgress();

}

void AAuraCharacter::LoadProgress()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameMode)
	{
		ULoadScreenSaveGame* SaveData = AuraGameMode->RetriveInGameSaveData();
		if (SaveData == nullptr)return;


		if (SaveData->bFirstTimeLoadIn)
		{
			InitializeDefualtAttributes();
			AddCharacterAbilities();//这里调用AuraCharacterBase中的AddCharacterAbilities()这个函数
		}
		else
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
			{
				AuraASC->AddCharacterAbilitiesFromSaveData(SaveData);
			}

			if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
			{
				AuraPlayerState->SetLevel(SaveData->PlayerLevel);
				AuraPlayerState->SetXP(SaveData->XP);
				AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
				AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
			}
			
			UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData);
		}
	}
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//Init Ability Actor Info for the Client
	InitAbilityActorInfo();
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
		AuraPlayerController->bShowMouseCursor = false;//更改鼠标是否显示 ， 必须要移动鼠标一点点位置才更新 这里我没做，  在电击怪的蓝图做过。
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
		AuraPlayerController->bShowMouseCursor = true;
	}
}

// 保存游戏进度到存档的实现函数
void AAuraCharacter::SaveProgress_Implementation(const FName& CheckPointTag)
{
	// 获取当前游戏的GameMode，并转换为自定义的AAuraGameModeBase类型
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	// 检查GameMode是否有效
	if (AuraGameMode)
	{
		// 从GameMode获取当前游戏的存档数据(非指定槽位的存档数据，所以这里用RetriveInGameSaveData）
		// RetriveInGameSaveData()会自动获取当前游戏会话的存档
		ULoadScreenSaveGame* SaveData = AuraGameMode->RetriveInGameSaveData();
		// 如果存档数据为空，直接返回（避免空指针异常）
		if (SaveData == nullptr)return;
		
		// 将传入的检查点标签(CheckPointTag)保存到存档数据中
		// 这通常用于记录玩家最后到达的检查点或重生点
		SaveData->PlayerStartTag = CheckPointTag;

		// 保存玩家状态相关数据
		if (const AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>())
		{
			SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveData->XP = AuraPlayerState->GetXP();
			SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
			SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
		}
		// 保存角色主要属性数值
		SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());


		/*当执行 SaveProgress_Implementation 时，说明玩家已经通过了游戏的初始阶段，并且游戏进度正在被保存（例如到达了一个检查点）。*/
		/*此时将 bFirstTimeLoadIn 设置为 false，
		表示：
			玩家已经完成了首次加载过程
			后续加载这个存档时，游戏不需要再执行首次加载的特殊逻辑（如初始教程、初始物品分配等）
			游戏可以正常从检查点恢复进度*/
		SaveData->bFirstTimeLoadIn = false;

		// 如果不是在服务器上，直接返回（确保只有服务器能保存能力数据）
		if (!HasAuthority())return;
		

		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		FForEachAbility SaveAbilityDelegate;

		SaveData->SavedAbilities.Empty();

		// 使用Lambda函数遍历所有能力并保存
		SaveAbilityDelegate.BindLambda
		([this, AuraASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
			{
				// 获取能力的GameplayTag
				const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFormSpec(AbilitySpec);
				// 从能力信息库中获取该能力的详细信息
				UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);

				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

				// 构建保存的能力数据结构
				FSavedAbility SavedAbility;
				SavedAbility.GameplayAbility = Info.Ability;
				SavedAbility.AbilityLevel = AbilitySpec.Level;
				SavedAbility.AbilitySlot = AuraASC->GetSlotTagFromAbilityTag(AbilityTag);
				SavedAbility.AbilityStatus = AuraASC->GetStatusFromAbilityTag(AbilityTag);
				SavedAbility.AbilityTag = AbilityTag;
				SavedAbility.AbilityType = Info.AbilityType;

				// 将能力数据添加到存档中
				SaveData->SavedAbilities.AddUnique(SavedAbility);
			}
		);
		// 遍历所有能力并应用上面的保存逻辑（代理的回调模式，不是广播模式）
		AuraASC->ForEachAbility(SaveAbilityDelegate);

		//保存"当前正在进行的游戏"的进度数据(SaveInGameProgressData)
		AuraGameMode->SaveInGameProgressData(SaveData);
	}
}

void AAuraCharacter::OnRep_Stunned()
{
	Super::OnRep_Stunned();

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPresssed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);

		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			//StunNiagaraComponent->Activate();
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			//StunNiagaraComponent->Deactivate();
		}
	}
}



int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level)const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level)const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(InPlayerLevel);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatus(AuraPlayerState->GetPlayerLevel());
	}
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelupParticles();
}

void AAuraCharacter::MulticastLevelupParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();

		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetXP_Implementation()const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::InitAbilityActorInfo()
{

	AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>();
	if (AuraPlayerState)

		//check(AuraPlayerState); check有大坑，因为PossessedBy函数调用的很早，第一次检查PlayerState的时候还没初始化，check就检查第一次就崩了
		// 但是PossessedBy会反复调用。等PlayerState初始化好了就得用if,if就是反复判断条件成立就会执行花括号内容。
	{
		AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
		Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
		AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
		AttributeSet = AuraPlayerState->GetAttributeSet();

		//初始化PrimaryAttribute和SecondaryAttribute
		//InitializeDefualtAttributes();
	
	}
	OnASCRegisteredDelegate.Broadcast(AbilitySystemComponent);

	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}


