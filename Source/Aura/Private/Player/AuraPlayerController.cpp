// Copyright Ivy


#include "Player/AuraPlayerController.h"
#include <../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include "Actor/MagicCircle.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Materials/MaterialInterface.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/DecalComponent.h>
#include "Interaction/HighlightInterface.h"
#include "../Aura.h"

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
		//在 Aura 项目中，AuraInputComponent->BindAbilityActions() 负责将 输入动作（Input Actions） 绑定到三个核心回调函数：

		//	AbilityInputTagPressed（按下时触发一次）
		//	AbilityInputTagReleased（释放时触发一次）
		//	AbilityInputTagHeld（按住时每帧触发）
	}
	else 
	{
		// 处理转换失败的情况，可能记录错误或警告
		UE_LOG(LogTemp, Error, TEXT("InputComponent is not an instance of UEnhancedInputComponent!"));
	}
}

void AAuraPlayerController::HightlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HighlightActor;
	}
}

void AAuraPlayerController::UnHightlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnHighlightActor;
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// 检查1：如果能力系统组件(ASC)存在，并且当前拥有阻止“输入按下”的标签（例如处于硬直、施法状态），则直接返回，不处理输入。
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPresssed))
	{
		return;
	}
	// 检查2：如果按下的输入标签是右键（RMB）
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		// 【核心逻辑】设置当前的瞄准状态(TargetingStatus)
		// 判断当前光标下的Actor（ThisActor）是否实现了 UEnemyInterface 接口。
		// - 如果实现了（是敌人），则状态设为 TargetingEnemy（瞄准敌人）。
		// - 如果没实现（不是敌人），则状态设为 TargetingNonEnemy（瞄准非敌人）。这包括友方NPC、可交互物、环境等。
		TargetingStatus = ThisActor->Implements<UEnemyInterface>() ? ETargetingStatus::TargetingEnemy : ETargetingStatus::TargetingNonEnemy;

		// 当按下右键时，取消自动移动状态。(bAutoRunning = false; 在按下瞬间的首要目的不是决定新操作，而是终止旧操作。语义：“不管我接下来要做什么（移动或攻击），先停止当前的自动移动再说。)
		bAutoRunning = false;
		/*所以，当您按下右键想移动时：

			您的第一次按下，首先起到了 “刹车”(bAutoRunning = false) 的作用。
			随后您的持续按住，起到了 “手动方向盘”(AddMovementInput) 的作用。
			或者您的快速点击释放，起到了 “设置导航目的地”(新的 bAutoRunning = true) 的作用。*/
	}



	// 最后，将“按下”事件传递给能力系统组件(ASC)，以便触发相应的GameplayAbility（例如，按下瞬间触发的技能）。
	if (GetASC())GetASC()->AbilityInputTagPressed(InputTag);
}

// 当某个绑定到能力的输入键被释放时调用
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// 检查1：如果ASC存在且拥有阻止“输入释放”的标签，则直接返回。
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	// 检查2：如果释放的键不是右键（RMB），则直接传递给ASC处理然后返回。
   // （例如，释放Q、E等按键，只触发能力，不涉及移动逻辑）
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return; // 注意这里的return，非RMB的处理到此结束
	}

	// 以下是专门处理右键释放的逻辑...
	// 首先，无论如何都先把“释放”事件传递给ASC（可能有些能力需要监听Release事件，如结束引导技能）
	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
	}

	// 【核心逻辑】关键判断：如果当前状态不是“瞄准敌人” 并且 没有按下Shift键
   // 这个条件意味着：只有当你瞄准的不是敌人（即TargetingNonEnemy状态）且没有强制站立（Shift）时，才处理移动逻辑。
   // 如果你瞄准的是敌人（TargetingEnemy），或者按下了Shift键，都会跳过移动逻辑。
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		// 获取当前控制的角色
		const APawn* ControlledPawn = GetPawn();
		// 判断是否为“短按”（按住时间小于阈值）并且角色有效
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// 获取光标下的命中结果
			FHitResult Hit;
			bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);

			// 如果点击的位置在导航网格上，直接使用点击的位置
			if (bHit)
			{
				// 缓存点击的位置作为目标点
				CachedDestination = Hit.ImpactPoint;
			}

			// 计算从角色当前位置到目标点的导航路径
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				GetWorld(),                      // 世界上下文
				ControlledPawn->GetActorLocation(), // 路径起点：角色位置
				CachedDestination               /// 路径终点：光标点击位置
			);

			// 如果路径计算成功且至少有一个路径点
			if (NavPath && NavPath->PathPoints.Num() > 0)
			{
				// 安全检查Spline组件是否有效（用于显示移动路径或控制自动移动）
				if (!IsValid(Spline))
				{
					UE_LOG(LogTemp, Error, TEXT("Spline is not valid!"));
					return;
				}
				// 清空Spline上已有的点
				Spline->ClearSplinePoints();

				// 将导航路径的每一个点添加到Spline中
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					// 跳过无效的位置（包含NaN）
					if (PointLoc.ContainsNaN())
					{
						continue;
					}

					// 在世界空间下添加Spline点
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					// 绘制调试球体，显示路径点（仅在开发版本可见）
					DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
				}
				// 将最终目标点更新为路径的最后一个点（导航网格上的精确点）
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];

				// 设置自动移动状态为true，角色将开始沿着Spline路径自动移动
				bAutoRunning = true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("NavPath is invalid or has no points!"));
			}
			// 如果当前有阻止“输入持有”的标签（可能用于控制何时播放特效），则在目标点生成点击特效
			if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
		}
		// 重置按住时间，为下一次按键做准备
		FollowTime = 0.f;

		// 重置瞄准状态为“未瞄准”    重置：在 AbilityInputTagReleased（释放右键后）处理完所有逻辑后，必须将其归位。为了保证状态机严谨和清晰而必须重置
		TargetingStatus = ETargetingStatus::NotTargeting;
	}
	// 如果条件不满足（瞄准了敌人或按着Shift），则什么都不做，只传递能力输入。
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// 检查1：如果ASC存在且当前有阻止输入持有的标签，则直接返回（关键点解释见下文）
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;// 直接退出，不处理任何输入
	}
	// 检查2：如果不是右键输入（RMB）的特殊情况
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (GetASC())
		{
			// 普通输入直接传递给ASC处理
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	// 右键输入的特殊处理分支
	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
	{
		// 处于瞄准状态或按住Shift时，当作普通能力输入处理
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		// 否则处理为移动输入（右键移动逻辑）
		FollowTime += GetWorld()->GetDeltaSeconds();
		// 获取光标下的命中位置
		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			// 缓存目标位置
			CachedDestination = Hit.ImpactPoint;
		}
		// 控制Pawn向目标位置移动
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}	
		
	}
	/*设计逻辑时序：
		按键按下 →Ability被激活 →ActivationOwnedTags 添加 Player_Block_InputHeld 标签 →后续输入被阻止,在技能解释（EndAbility)后该Tag才取消。
		现在这个代码情况是技能激活时（GA_Electrocute）禁用所有输入
	  潜在问题:
			竞态条件：如果同一个帧内有多个输入（如快速连续按键），可能会在标签生效前漏过部分输入。
			技能能否激活：Player_Block_InputHeld 是否会阻止自身的激活？取决于：
			如果标签通过 AbilityTags 或 BlockAbilitiesWithTag 配置，可能会阻止自身。
			如果是纯逻辑判断（如你的代码），则不会阻止自身（因为标签尚未添加）。
	        直接用 Player_Block_InputHeld 全局阻止输入的方式确实会破坏蓄力技能的逻辑，因为蓄力技能需要持续接收输入事件（Held），而你的当前设计会在技能激活后立即阻止所有后续输入。

	改进方案，根据项目需求选择：
	方案1：区分输入阻止类型（推荐）
		核心思想：不用全局标签阻止所有输入，而是分类控制。
	方案2：动态标签管理
        核心思想：在蓄力技能中动态控制标签的添加/移除。
	方案3：白名单机制
	    核心思想：允许特定技能的输入绕过阻止逻辑。*/
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
	UpdateMagicCircleLocation();

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


void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (GetASC()&&GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		UnHightlightActor(LastActor);
		UnHightlightActor(ThisActor);
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}
	

	// 调用GetHitResultUnderCursor函数，尝试获取光标下的物体碰撞信息。
	// CursorHit用于存储函数返回的结果。
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);

	// 检查CursorHit.bBlockingHit是否为false，即是否没有发生阻挡碰撞。
	if (!CursorHit.bBlockingHit)return;


	// 如果发生了阻挡碰撞，更新LastActor为之前的ThisActor。
    // 这里LastActor用于存储上一次选中的怪物。
	LastActor = ThisActor;

	// 如果转换成功，ThisActor将指向一个新的怪物对象；如果失败，则ThisActor将为nullptr。
	if (IsValid(CursorHit.GetActor()) && CursorHit.GetActor()->Implements<UHighlightInterface>())
	{
		ThisActor = CursorHit.GetActor();
	}
	else
	{
		ThisActor = nullptr;
	}


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
	if (LastActor!=ThisActor)
	{
		UnHightlightActor(LastActor);
		HightlightActor(ThisActor);
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

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
		}
	}
	
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}
