// Copyright Ivy


#include "Character/AuraCharacter.h"

AAuraCharacter::AAuraCharacter()
{
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
	//�������AuraCharacterBase�е�AddCharacterAbilities()�������
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//Init Ability Actor Info for the Client
	InitAbilityActorInfo();
	
	
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
		InitializeDefualtAttributes();
	
	}
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}
