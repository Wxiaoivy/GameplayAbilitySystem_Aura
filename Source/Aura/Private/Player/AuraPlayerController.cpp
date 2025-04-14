// Copyright Ivy


#include "Player/AuraPlayerController.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");

}



void AAuraPlayerController::BeginPlay()

{
	Super::BeginPlay();

	check(AuraContext);


	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	//check(Subsystem);
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = EMouseCursor::Default;

	//FInputModeGameAndUI��һ���ṹ�壨���࣬����ȡ����Unreal Engine���ڲ�ʵ�֣�������װ������Ϸ��UI������ص����á�
	FInputModeGameAndUI InputModeData;



	//ͨ��InputModeData����SetLockMouseToViewportBehavior����
	//���������������������ӿڣ�viewport������Ϊ��
	// EMouseLockMode::DoNotLock��һ��ö��ֵ����ʾ��겻Ӧ�ñ��������ӿ��ڡ�����ζ����ҿ��������ƶ���꣬�����ᱻ��������Ϸ�����ڡ�
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);


	//ͨ��InputModeData����SetHideCursorDuringCapture������������false��Ϊ������
	// ����������õ����뱻����ʱ�Ƿ����ع�ꡣ����false��ζ�ż�ʹ���뱻���񣬹��Ҳ���ᱻ���ء�
	InputModeData.SetHideCursorDuringCapture(false);

	//SetInputMode��һ������������˵��һ�������
	// ���������Ǹ��ߵ��ԣ����٣���������һ�������������뷽ʽ����
	// 
	//InputModeData��һ�������˾���������Ϣ�����ݰ���������ݰ�����ܰ����˺ܶ���Ϣ��
	// ���磺�����Ҫ��Ҫ��������Ϸ���������������Ϸ������ʱ��Ҫ��Ҫ��������ꣿ���ȵȡ�
	SetInputMode(InputModeData);

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Ϊʲô����������ȷ��InputComponent����UEnhancedInputComponent   ��Ϊ��UE�༭��������Input��Ĭ�Ϸ�ʽ������ǿ����

	UAuraInputComponent* AuraInputComponent = Cast<UAuraInputComponent>(InputComponent);
	if (AuraInputComponent)
	{

		AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
		AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
		AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);

		AuraInputComponent->BindAbilityActions(InputConfig,this,&ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);

	}
	else 
	{
		// ����ת��ʧ�ܵ���������ܼ�¼����򾯸�
		UE_LOG(LogTemp, Error, TEXT("InputComponent is not an instance of UEnhancedInputComponent!"));
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}
	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
	}
	if (!bTargeting && !bShiftKeyDown)
	{
		APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// ��ȡ���λ��
			FHitResult Hit;
			bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);

			// ��������λ���ڵ��������ϣ�ֱ��ʹ�õ����λ��
			if (bHit)
			{
				CachedDestination = Hit.ImpactPoint;
			}
			// ����·��
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				GetWorld(),                      // WorldContextObject
				ControlledPawn->GetActorLocation(), // Start Location
				CachedDestination               // End Location
			);

			// ���� Spline
			if (NavPath && NavPath->PathPoints.Num() > 0)
			{
				if (!IsValid(Spline))
				{
					UE_LOG(LogTemp, Error, TEXT("Spline is not valid!"));
					return;
				}

				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					if (PointLoc.ContainsNaN())
					{
						continue;
					}

					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
				}
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				bAutoRunning = true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("NavPath is invalid or has no points!"));
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
	
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	if (bTargeting||bShiftKeyDown)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			CachedDestination = Hit.ImpactPoint;
		}
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
		
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{

	if (AuraAbilitySystemComponent== nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}


void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();

}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)return;
	if (APawn* ControlledPawn = GetPawn())
	{
		if (!IsValid(Spline))
		{
			UE_LOG(LogTemp, Error, TEXT("Spline is not valid!"));
			return;
		}
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		UE_LOG(LogTemp, Log, TEXT("Distance to destination: %f"), DistanceToDestination);
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
			UE_LOG(LogTemp, Log, TEXT("AutoRun stopped."));
		}
	}
}


void AAuraPlayerController::CursorTrace()
{
	// ����һ��FHitResult���͵ı���CursorHit�����ڴ洢���׷�ٵĽ����
	FHitResult CursorHit;

	// ����GetHitResultUnderCursor���������Ի�ȡ����µ�������ײ��Ϣ��
	// CursorHit���ڴ洢�������صĽ����
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// ���CursorHit.bBlockingHit�Ƿ�Ϊfalse�����Ƿ�û�з����赲��ײ��
	if (!CursorHit.bBlockingHit)return;

	// ����������赲��ײ������LastActorΪ֮ǰ��ThisActor��
    // �������LastActor���ڴ洢��һ��ѡ�еĹ��
	LastActor = ThisActor;

	// ���ת���ɹ���ThisActor��ָ��һ���µĹ���������ʧ�ܣ���ThisActor��Ϊnullptr��
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());


	/*
	* Line trace from cursor. There are several scenarios:
	* 
	A. LastActor is null && ThisActor is null
	- Do nothing

	B. LastActor is null && ThisActor is valid
    - Highlight ThisActor

    C. LastActor is valid && ThisActor is null
	-UnHighlight LastActor

	D. Both actors are valid, but LastActor != ThisActor
	-UnHighlight LastActor, and Highlight ThisActor

	E. Both actors are valid, and are the same actor
    - Do nothing

	*/
	if (LastActor==nullptr)
	{
		if (ThisActor) 
		{
			//Case B :
			ThisActor->HighlightActor();
		}
		else
		{
			//Case A : Do nothing
		}
	}
	else 
	{
		if (ThisActor)
		{
			if (LastActor != ThisActor)
			{
				//Case D :
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				//Case E : Do nothing
			}
		}
		else
		{
			//Case C :
			LastActor->UnHighlightActor();

		}
	}
}





void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	//�����ά�������������û������Value.
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//��Controller����ת�����㷽��
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);


	//����ķ���ʹ�õ�����ά����������ǰ����X���ҷ���Y
	//RotationMatrix�����ڱ�ʾ�ͼ���3D�ռ��е���ת��������������ת��Ϸ���󡢵�������ӽǡ�ģ�������˶��ȡ�
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);



	//���д���������ǳ��Դӵ�ǰ��������ȡ�������Ƶ�pawn���󣬲�����ȡ����pawn�����ָ��洢��ControlledPawn�����С�
	// ����ɹ���ȡ����pawn������ô��ִ��if�����ڵĴ��룻���򣬽������ô���顣
	//GetPawn<APawn>()������һ��ģ�庯�������ڴӵ�ǰ��������ȡ�������Ƶ�pawn����GetPawn����ͨ��������AController���С�
	if (APawn* ControlledPawn = GetPawn<APawn>())	
	{

	    //�����InputAxisVector.Y ��(��������)��Y���ϵķ������������������Ҫǰ���ƶ�������
	    //�����������AddMovementInput�����ĵڶ�����������ForwardDirection������ת���X�᷽��Ҳ����ǰ�������ϣ�
	    // ��ȷ����ɫ������Ӧ�������ĸ������ƶ��Լ��ƶ�������
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

	}

}

void AAuraPlayerController::ShowDamageNumber(float Damage, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{

	if (IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageTextComponent = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageTextComponent->RegisterComponent();
		DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageTextComponent->SetDamageText(Damage, bBlockedHit, bCriticalHit);
		
		
	}
}