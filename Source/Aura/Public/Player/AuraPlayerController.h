// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputModule.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "Interaction/EnemyInterface.h"
#include "AuraPlayerController.generated.h"


/**
 * 
 */
class UInputMappingContext;


UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	
protected:
	virtual void  BeginPlay()override;

	virtual void SetupInputComponent()override;

	virtual void PlayerTick(float DeltaTime)override;
	

private:
	//声明一个UInputMappingContext类型的指针，在UE编辑器里设置已经做好了的IMC
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext>AuraContext;

	//声明一个UInputAction类型的指针，在UE编辑器里设置已经做好了的IA
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>MoveAction;

	void Move(const FInputActionValue& InputActionValue);	

	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;

};
