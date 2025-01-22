// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AuraCharacter.h"
//#include "EnhancedInputModule.h"
//#include "InputActionValue.h"
//#include "EnhancedInputComponent.h"
//#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "MyPlayerController.generated.h"


/**
 * 
 */
class UInputMappingContext;


UCLASS()
class AURA_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMyPlayerController();
	
protected:
	virtual void  BeginPlay()override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);


private:
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext>AuraContext;

	
};
