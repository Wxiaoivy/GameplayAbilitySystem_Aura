// Copyright Ivy


#include "Player/MyPlayerController.h"

AMyPlayerController::AMyPlayerController()
{
	bReplicates = true;

}

void AMyPlayerController::BeginPlay()

{
	Super::BeginPlay();
	check(AuraContext);

    //UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

}

void AMyPlayerController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
}
