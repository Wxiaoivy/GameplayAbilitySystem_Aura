// Copyright Ivy


#include "Player/AuraPlayerController.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

}

void AAuraPlayerController::BeginPlay()

{
	Super::BeginPlay();

	check(AuraContext);


	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraContext, 0);

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

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	}
	else {
		// ����ת��ʧ�ܵ���������ܼ�¼����򾯸�
		UE_LOG(LogTemp, Error, TEXT("InputComponent is not an instance of UEnhancedInputComponent!"));
	}



}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();


}


void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)return;

	LastActor = ThisActor;
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

