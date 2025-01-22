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

