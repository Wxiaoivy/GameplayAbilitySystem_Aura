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
			AddCharacterAbilities();//�������AuraCharacterBase�е�AddCharacterAbilities()�������
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
		AuraPlayerController->bShowMouseCursor = false;//��������Ƿ���ʾ �� ����Ҫ�ƶ����һ���λ�òŸ��� ������û����  �ڵ���ֵ���ͼ������
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

// ������Ϸ���ȵ��浵��ʵ�ֺ���
void AAuraCharacter::SaveProgress_Implementation(const FName& CheckPointTag)
{
	// ��ȡ��ǰ��Ϸ��GameMode����ת��Ϊ�Զ����AAuraGameModeBase����
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	// ���GameMode�Ƿ���Ч
	if (AuraGameMode)
	{
		// ��GameMode��ȡ��ǰ��Ϸ�Ĵ浵����(��ָ����λ�Ĵ浵���ݣ�����������RetriveInGameSaveData��
		// RetriveInGameSaveData()���Զ���ȡ��ǰ��Ϸ�Ự�Ĵ浵
		ULoadScreenSaveGame* SaveData = AuraGameMode->RetriveInGameSaveData();
		// ����浵����Ϊ�գ�ֱ�ӷ��أ������ָ���쳣��
		if (SaveData == nullptr)return;
		
		// ������ļ����ǩ(CheckPointTag)���浽�浵������
		// ��ͨ�����ڼ�¼�����󵽴�ļ����������
		SaveData->PlayerStartTag = CheckPointTag;

		// �������״̬�������
		if (const AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>())
		{
			SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveData->XP = AuraPlayerState->GetXP();
			SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
			SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
		}
		// �����ɫ��Ҫ������ֵ
		SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());


		/*��ִ�� SaveProgress_Implementation ʱ��˵������Ѿ�ͨ������Ϸ�ĳ�ʼ�׶Σ�������Ϸ�������ڱ����棨���絽����һ�����㣩��*/
		/*��ʱ�� bFirstTimeLoadIn ����Ϊ false��
		��ʾ��
			����Ѿ�������״μ��ع���
			������������浵ʱ����Ϸ����Ҫ��ִ���״μ��ص������߼������ʼ�̡̳���ʼ��Ʒ����ȣ�
			��Ϸ���������Ӽ���ָ�����*/
		SaveData->bFirstTimeLoadIn = false;

		// ��������ڷ������ϣ�ֱ�ӷ��أ�ȷ��ֻ�з������ܱ����������ݣ�
		if (!HasAuthority())return;
		

		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		FForEachAbility SaveAbilityDelegate;

		SaveData->SavedAbilities.Empty();

		// ʹ��Lambda����������������������
		SaveAbilityDelegate.BindLambda
		([this, AuraASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
			{
				// ��ȡ������GameplayTag
				const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFormSpec(AbilitySpec);
				// ��������Ϣ���л�ȡ����������ϸ��Ϣ
				UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);

				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

				// ����������������ݽṹ
				FSavedAbility SavedAbility;
				SavedAbility.GameplayAbility = Info.Ability;
				SavedAbility.AbilityLevel = AbilitySpec.Level;
				SavedAbility.AbilitySlot = AuraASC->GetSlotTagFromAbilityTag(AbilityTag);
				SavedAbility.AbilityStatus = AuraASC->GetStatusFromAbilityTag(AbilityTag);
				SavedAbility.AbilityTag = AbilityTag;
				SavedAbility.AbilityType = Info.AbilityType;

				// ������������ӵ��浵��
				SaveData->SavedAbilities.AddUnique(SavedAbility);
			}
		);
		// ��������������Ӧ������ı����߼�������Ļص�ģʽ�����ǹ㲥ģʽ��
		AuraASC->ForEachAbility(SaveAbilityDelegate);

		//����"��ǰ���ڽ��е���Ϸ"�Ľ�������(SaveInGameProgressData)
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

		//check(AuraPlayerState); check�д�ӣ���ΪPossessedBy�������õĺ��磬��һ�μ��PlayerState��ʱ��û��ʼ����check�ͼ���һ�ξͱ���
		// ����PossessedBy�ᷴ�����á���PlayerState��ʼ�����˾͵���if,if���Ƿ����ж����������ͻ�ִ�л��������ݡ�
	{
		AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
		Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
		AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
		AttributeSet = AuraPlayerState->GetAttributeSet();

		//��ʼ��PrimaryAttribute��SecondaryAttribute
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


