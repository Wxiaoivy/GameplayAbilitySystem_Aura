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

}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//Init Ability Actor Info for the Server
	InitAbilityActorInfo();

}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//Init Ability Actor Info for the Client
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	/*
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		check(PlayerController);*/

	if (AAuraPlayerState* AuraPlayerState = GetPlayerState <AAuraPlayerState>())

		//check(AuraPlayerState); check�д�ӣ���ΪPossessedBy�������õĺ��磬��һ�μ��PlayerState��ʱ��û��ʼ����check�ͼ���һ�ξͱ���
		// ����PossessedBy�ᷴ�����á���PlayerState��ʼ�����˾͵���if,if���Ƿ����ж����������ͻ�ִ�л��������ݡ�
	{
		AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
		AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
		AttributeSet = AuraPlayerState->GetAttributeSet();
	}
}
