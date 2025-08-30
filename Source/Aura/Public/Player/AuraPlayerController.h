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
class IHighlightInterface;

enum class ETargetingStatus : uint8
{
	// ״̬��������׼����
	// ��ʾ��굱ǰָ��һ����ʶ��Ϊ"����"�Ŀɽ�������
	TargetingEnemy, 

	// ״̬��������׼�ǵ���
	// ��ʾ��굱ǰָ��һ���ɽ������󣬵��ö����ǵ���
	// ���������ѷ�NPC��������λ���ɽ�����������ȣ�
	TargetingNonEnemy,

	// ״̬��δ��׼�κ��ض�Ŀ��
	// ��ʾ���ָ����ǲ��ɽ����Ļ���������桢ǽ�ڣ�
	// ����û��ָ���κ���Ч����Ϸ����
	NotTargeting
};

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();


	 /*Client RPC �����ϸ���ѭ _Implementation + _Validate ����������.
		ʹ�� WithValidation ʱ������ʵ����֤������
		�����б���������ʵ���б�����ȫһ�¡�*/
	UFUNCTION(Client, Reliable, WithValidation)//ShowDamageNumber �� Client RPC��ֻ���ڷ��������ò�ͬ�����ͻ��ˡ�
	void ShowDamageNumber(float Damage, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);//���˺������ڿͻ���û��ʾ ��û������ʲôԭ��

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

	
protected:
	virtual void  BeginPlay()override;

	virtual void SetupInputComponent()override;

	virtual void PlayerTick(float DeltaTime)override;

	
	

private:
	//����һ��UInputMappingContext���͵�ָ�룬��UE�༭���������Ѿ������˵�IMC
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext>AuraContext;

	//����һ��UInputAction���͵�ָ�룬��UE�༭���������Ѿ������˵�IA
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
	TObjectPtr<AActor> LastActor;// ��һ֡�Ĺ����ָ�Ŀɽ�������
	TObjectPtr<AActor> ThisActor;// ��ǰ֡����һ֡���Ĺ����ָ�Ŀɽ�������

	// ����һ��FHitResult���͵ı���CursorHit�����ڴ洢���׷�ٵĽ����
	FHitResult CursorHit;

	static void HightlightActor(AActor* InActor);
	static void UnHightlightActor(AActor* InActor);

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
	//bool bTargeting = false;
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;


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
