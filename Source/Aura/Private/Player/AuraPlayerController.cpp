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

	//FInputModeGameAndUI是一个结构体（或类，具体取决于Unreal Engine的内部实现），它封装了与游戏和UI输入相关的设置。
	FInputModeGameAndUI InputModeData;



	//通过InputModeData调用SetLockMouseToViewportBehavior方法
	//这个方法设置鼠标锁定到视口（viewport）的行为。
	// EMouseLockMode::DoNotLock是一个枚举值，表示鼠标不应该被锁定到视口内。这意味着玩家可以自由移动鼠标，而不会被限制在游戏窗口内。
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);


	//通过InputModeData调用SetHideCursorDuringCapture方法，并传递false作为参数。
	// 这个方法设置当输入被捕获时是否隐藏光标。传递false意味着即使输入被捕获，光标也不会被隐藏。
	InputModeData.SetHideCursorDuringCapture(false);

	//SetInputMode是一个函数（或者说是一个命令），
	// 它的作用是告诉电脑：“嘿，我想设置一下这个程序的输入方式。”
	// 
	//InputModeData是一个包含了具体设置信息的数据包。这个数据包里可能包含了很多信息，
	// 比如：“鼠标要不要锁定在游戏窗口里？”、“当游戏在运行时，要不要隐藏鼠标光标？”等等。
	SetInputMode(InputModeData);

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//为什么这里外面能确定InputComponent就是UEnhancedInputComponent   因为在UE编辑器里设置Input的默认方式就是增强输入

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
		// 处理转换失败的情况，可能记录错误或警告
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
			// 获取点击位置
			FHitResult Hit;
			bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);

			// 如果点击的位置在导航网格上，直接使用点击的位置
			if (bHit)
			{
				CachedDestination = Hit.ImpactPoint;
			}
			// 计算路径
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				GetWorld(),                      // WorldContextObject
				ControlledPawn->GetActorLocation(), // Start Location
				CachedDestination               // End Location
			);

			// 更新 Spline
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
	// 定义一个FHitResult类型的变量CursorHit，用于存储光标追踪的结果。
	FHitResult CursorHit;

	// 调用GetHitResultUnderCursor函数，尝试获取光标下的物体碰撞信息。
	// CursorHit用于存储函数返回的结果。
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// 检查CursorHit.bBlockingHit是否为false，即是否没有发生阻挡碰撞。
	if (!CursorHit.bBlockingHit)return;

	// 如果发生了阻挡碰撞，更新LastActor为之前的ThisActor。
    // 这里假设LastActor用于存储上一次选中的怪物。
	LastActor = ThisActor;

	// 如果转换成功，ThisActor将指向一个新的怪物对象；如果失败，则ThisActor将为nullptr。
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
	//这个二维向量拿来储存用户输入的Value.
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//用Controller的旋转来计算方向
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);


	//这里的方向使用的是三维向量，所以前方是X，右方是Y
	//RotationMatrix常用于表示和计算3D空间中的旋转。它可以用于旋转游戏对象、调整相机视角、模拟物理运动等。
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);



	//这行代码的作用是尝试从当前控制器获取它所控制的pawn对象，并将获取到的pawn对象的指针存储在ControlledPawn变量中。
	// 如果成功获取到了pawn对象，那么将执行if语句块内的代码；否则，将跳过该代码块。
	//GetPawn<APawn>()：这是一个模板函数，用于从当前控制器获取它所控制的pawn对象。GetPawn函数通常定义在AController类中。
	if (APawn* ControlledPawn = GetPawn<APawn>())	
	{

	    //在这里，InputAxisVector.Y 是(输入向量)在Y轴上的分量，它代表了玩家想要前后移动的量。
	    //这个量被用作AddMovementInput方法的第二个参数，与ForwardDirection（即旋转后的X轴方向，也就是前方）相结合，
	    // 来确定角色或物体应该沿着哪个方向移动以及移动多少量
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

	}

}

bool AAuraPlayerController::ShowDamageNumber_Validate(float Damage, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	return Damage >= 0 && IsValid(TargetCharacter);
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float Damage, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
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