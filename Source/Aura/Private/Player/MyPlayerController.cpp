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

   UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
   check(Subsystem);
   Subsystem->AddMappingContext(AuraContext, 0);

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

