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
#include "UI/Widget/DamageTextComponent.h"
#include "AuraPlayerController.generated.h"


/**
 * 
 */
class UInputMappingContext;
class AMagicCircle;


UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();


	 /*Client RPC 必须严格遵循 _Implementation + _Validate 的命名规则.
		使用 WithValidation 时，必须实现验证函数。
		参数列表在声明和实现中必须完全一致。*/
	UFUNCTION(Client, Reliable, WithValidation)//ShowDamageNumber 是 Client RPC，只能在服务器调用并同步到客户端。
	void ShowDamageNumber(float Damage, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);//该伤害数字在客户端没显示 还没检查出来什么原因

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

	
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

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>ShiftAction;


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	void Move(const FInputActionValue& InputActionValue);	
	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };;
	bool bShiftKeyDown = false;

	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;


	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY()
	UAuraAbilitySystemComponent* AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	// 定义一个FHitResult类型的变量CursorHit，用于存储光标追踪的结果。
	FHitResult CursorHit;
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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent>DamageTextComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem>ClickNiagaraSystem;

	UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AMagicCircle>MagicCircleClass;

	UPROPERTY()
	TObjectPtr<AMagicCircle>MagicCircle;

	void UpdateMagicCircleLocation();
};
