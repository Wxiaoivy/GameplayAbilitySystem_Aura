// Copyright Ivy


#include "UI/HUD/AuraHUD.h"


UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		//OverlayWidgetController->BindCallbacksToDependencies();


		return OverlayWidgetController;

	}

	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
		return AttributeMenuWidgetController;
	}

	return AttributeMenuWidgetController;

}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	  //checkf(OverlayWidget, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
	    checkf(OverlayWidgetClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));

		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
		OverlayWidget = Cast<UAuraUserWidget>(Widget);

		//check��Ӧ�÷���ǰ��  ��ǰ�涼��û��ʼ����OverlayWidget = Cast<UAuraUserWidget>(Widget)��仰�Ÿ�OverlayWidget��ֵ ���Է�����仰����
		
		checkf(OverlayWidget, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
		//checkf(OverlayWidgetClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));

		const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
		UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

		OverlayWidget->SetWidgetController(WidgetController);

		WidgetController->BroadCastInitialValues();
		WidgetController->BindCallbacksToDependencies();//��ʦд��GetOverlayWidgetController����� ��д������� ���Թ�һ����Ч��
		Widget->AddToViewport();

	
    

	
}


