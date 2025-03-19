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
#include "Input/AuraInputConfig.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/SplineComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
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


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	void Move(const FInputActionValue& InputActionValue);	

	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;


	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY()
	UAuraAbilitySystemComponent* AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;


	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USplineComponent>Spline;


	void AutoRun();

};
