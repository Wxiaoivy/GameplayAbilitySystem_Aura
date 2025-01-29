// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */

 // ����һ���̳���UUserWidget���࣬�����Զ����û�����С������
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// һ����������ͼ��Unreal Engine�Ŀ��ӻ��ű�ϵͳ���е��õĺ�������������WidgetController��
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	// һ������ͼ��ֻ�������ԣ����ڴ洢WidgetController����
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject>WidgetController;

protected:
	// һ����������ͼ�б���д���¼���������SetWidgetController()������ʱ���á�
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

	
};
