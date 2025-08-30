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
		//�� Aura ��Ŀ�У�AuraInputComponent->BindAbilityActions() ���� ���붯����Input Actions�� �󶨵��������Ļص�������

		//	AbilityInputTagPressed������ʱ����һ�Σ�
		//	AbilityInputTagReleased���ͷ�ʱ����һ�Σ�
		//	AbilityInputTagHeld����סʱÿ֡������
	}
	else 
	{
		// ����ת��ʧ�ܵ���������ܼ�¼����򾯸�
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
	// ���1���������ϵͳ���(ASC)���ڣ����ҵ�ǰӵ����ֹ�����밴�¡��ı�ǩ�����紦��Ӳֱ��ʩ��״̬������ֱ�ӷ��أ����������롣
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPresssed))
	{
		return;
	}
	// ���2��������µ������ǩ���Ҽ���RMB��
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		// �������߼������õ�ǰ����׼״̬(TargetingStatus)
		// �жϵ�ǰ����µ�Actor��ThisActor���Ƿ�ʵ���� UEnemyInterface �ӿڡ�
		// - ���ʵ���ˣ��ǵ��ˣ�����״̬��Ϊ TargetingEnemy����׼���ˣ���
		// - ���ûʵ�֣����ǵ��ˣ�����״̬��Ϊ TargetingNonEnemy����׼�ǵ��ˣ���������ѷ�NPC���ɽ���������ȡ�
		TargetingStatus = ThisActor->Implements<UEnemyInterface>() ? ETargetingStatus::TargetingEnemy : ETargetingStatus::TargetingNonEnemy;

		// �������Ҽ�ʱ��ȡ���Զ��ƶ�״̬��(bAutoRunning = false; �ڰ���˲�����ҪĿ�Ĳ��Ǿ����²�����������ֹ�ɲ��������壺�������ҽ�����Ҫ��ʲô���ƶ��򹥻�������ֹͣ��ǰ���Զ��ƶ���˵��)
		bAutoRunning = false;
		/*���ԣ����������Ҽ����ƶ�ʱ��

			���ĵ�һ�ΰ��£��������� ��ɲ����(bAutoRunning = false) �����á�
			������ĳ�����ס������ ���ֶ������̡�(AddMovementInput) �����á�
			�������Ŀ��ٵ���ͷţ����� �����õ���Ŀ�ĵء�(�µ� bAutoRunning = true) �����á�*/
	}



	// ��󣬽������¡��¼����ݸ�����ϵͳ���(ASC)���Ա㴥����Ӧ��GameplayAbility�����磬����˲�䴥���ļ��ܣ���
	if (GetASC())GetASC()->AbilityInputTagPressed(InputTag);
}

// ��ĳ���󶨵���������������ͷ�ʱ����
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// ���1�����ASC������ӵ����ֹ�������ͷš��ı�ǩ����ֱ�ӷ��ء�
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	// ���2������ͷŵļ������Ҽ���RMB������ֱ�Ӵ��ݸ�ASC����Ȼ�󷵻ء�
   // �����磬�ͷ�Q��E�Ȱ�����ֻ�������������漰�ƶ��߼���
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return; // ע�������return����RMB�Ĵ����˽���
	}

	// ������ר�Ŵ����Ҽ��ͷŵ��߼�...
	// ���ȣ�������ζ��Ȱѡ��ͷš��¼����ݸ�ASC��������Щ������Ҫ����Release�¼���������������ܣ�
	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
	}

	// �������߼����ؼ��жϣ������ǰ״̬���ǡ���׼���ˡ� ���� û�а���Shift��
   // ���������ζ�ţ�ֻ�е�����׼�Ĳ��ǵ��ˣ���TargetingNonEnemy״̬����û��ǿ��վ����Shift��ʱ���Ŵ����ƶ��߼���
   // �������׼���ǵ��ˣ�TargetingEnemy�������߰�����Shift�������������ƶ��߼���
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		// ��ȡ��ǰ���ƵĽ�ɫ
		const APawn* ControlledPawn = GetPawn();
		// �ж��Ƿ�Ϊ���̰�������סʱ��С����ֵ�����ҽ�ɫ��Ч
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// ��ȡ����µ����н��
			FHitResult Hit;
			bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);

			// ��������λ���ڵ��������ϣ�ֱ��ʹ�õ����λ��
			if (bHit)
			{
				// ��������λ����ΪĿ���
				CachedDestination = Hit.ImpactPoint;
			}

			// ����ӽ�ɫ��ǰλ�õ�Ŀ���ĵ���·��
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				GetWorld(),                      // ����������
				ControlledPawn->GetActorLocation(), // ·����㣺��ɫλ��
				CachedDestination               /// ·���յ㣺�����λ��
			);

			// ���·������ɹ���������һ��·����
			if (NavPath && NavPath->PathPoints.Num() > 0)
			{
				// ��ȫ���Spline����Ƿ���Ч��������ʾ�ƶ�·��������Զ��ƶ���
				if (!IsValid(Spline))
				{
					UE_LOG(LogTemp, Error, TEXT("Spline is not valid!"));
					return;
				}
				// ���Spline�����еĵ�
				Spline->ClearSplinePoints();

				// ������·����ÿһ������ӵ�Spline��
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					// ������Ч��λ�ã�����NaN��
					if (PointLoc.ContainsNaN())
					{
						continue;
					}

					// ������ռ������Spline��
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					// ���Ƶ������壬��ʾ·���㣨���ڿ����汾�ɼ���
					DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
				}
				// ������Ŀ������Ϊ·�������һ���㣨���������ϵľ�ȷ�㣩
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];

				// �����Զ��ƶ�״̬Ϊtrue����ɫ����ʼ����Spline·���Զ��ƶ�
				bAutoRunning = true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("NavPath is invalid or has no points!"));
			}
			// �����ǰ����ֹ��������С��ı�ǩ���������ڿ��ƺ�ʱ������Ч��������Ŀ������ɵ����Ч
			if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
		}
		// ���ð�סʱ�䣬Ϊ��һ�ΰ�����׼��
		FollowTime = 0.f;

		// ������׼״̬Ϊ��δ��׼��    ���ã��� AbilityInputTagReleased���ͷ��Ҽ��󣩴����������߼��󣬱��뽫���λ��Ϊ�˱�֤״̬���Ͻ�����������������
		TargetingStatus = ETargetingStatus::NotTargeting;
	}
	// ������������㣨��׼�˵��˻���Shift������ʲô��������ֻ�����������롣
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// ���1�����ASC�����ҵ�ǰ����ֹ������еı�ǩ����ֱ�ӷ��أ��ؼ�����ͼ����ģ�
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;// ֱ���˳����������κ�����
	}
	// ���2����������Ҽ����루RMB�����������
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (GetASC())
		{
			// ��ͨ����ֱ�Ӵ��ݸ�ASC����
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	// �Ҽ���������⴦���֧
	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
	{
		// ������׼״̬��סShiftʱ��������ͨ�������봦��
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		// ������Ϊ�ƶ����루�Ҽ��ƶ��߼���
		FollowTime += GetWorld()->GetDeltaSeconds();
		// ��ȡ����µ�����λ��
		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			// ����Ŀ��λ��
			CachedDestination = Hit.ImpactPoint;
		}
		// ����Pawn��Ŀ��λ���ƶ�
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}	
		
	}
	/*����߼�ʱ��
		�������� ��Ability������ ��ActivationOwnedTags ��� Player_Block_InputHeld ��ǩ ���������뱻��ֹ,�ڼ��ܽ��ͣ�EndAbility)���Tag��ȡ����
		���������������Ǽ��ܼ���ʱ��GA_Electrocute��������������
	  Ǳ������:
			��̬���������ͬһ��֡���ж�����루��������������������ܻ��ڱ�ǩ��Чǰ©���������롣
			�����ܷ񼤻Player_Block_InputHeld �Ƿ����ֹ����ļ��ȡ���ڣ�
			�����ǩͨ�� AbilityTags �� BlockAbilitiesWithTag ���ã����ܻ���ֹ����
			����Ǵ��߼��жϣ�����Ĵ��룩���򲻻���ֹ������Ϊ��ǩ��δ��ӣ���
	        ֱ���� Player_Block_InputHeld ȫ����ֹ����ķ�ʽȷʵ���ƻ��������ܵ��߼�����Ϊ����������Ҫ�������������¼���Held��������ĵ�ǰ��ƻ��ڼ��ܼ����������ֹ���к������롣

	�Ľ�������������Ŀ����ѡ��
	����1������������ֹ���ͣ��Ƽ���
		����˼�룺����ȫ�ֱ�ǩ��ֹ�������룬���Ƿ�����ơ�
	����2����̬��ǩ����
        ����˼�룺�����������ж�̬���Ʊ�ǩ�����/�Ƴ���
	����3������������
	    ����˼�룺�����ض����ܵ������ƹ���ֹ�߼���*/
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
	

	// ����GetHitResultUnderCursor���������Ի�ȡ����µ�������ײ��Ϣ��
	// CursorHit���ڴ洢�������صĽ����
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);

	// ���CursorHit.bBlockingHit�Ƿ�Ϊfalse�����Ƿ�û�з����赲��ײ��
	if (!CursorHit.bBlockingHit)return;


	// ����������赲��ײ������LastActorΪ֮ǰ��ThisActor��
    // ����LastActor���ڴ洢��һ��ѡ�еĹ��
	LastActor = ThisActor;

	// ���ת���ɹ���ThisActor��ָ��һ���µĹ���������ʧ�ܣ���ThisActor��Ϊnullptr��
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
