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

		//check不应该放最前面  最前面都还没初始化，OverlayWidget = Cast<UAuraUserWidget>(Widget)这句话才给OverlayWidget赋值 所以放在这句话后面
		
		checkf(OverlayWidget, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
		//checkf(OverlayWidgetClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));

		const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
		UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

		OverlayWidget->SetWidgetController(WidgetController);

		WidgetController->BroadCastInitialValues();
		WidgetController->BindCallbacksToDependencies();//老师写在GetOverlayWidgetController里面的 我写在这里的 测试过一样的效果
		Widget->AddToViewport();

	
    

	
}


