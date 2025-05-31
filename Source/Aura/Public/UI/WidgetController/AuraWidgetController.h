// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);//����һ����ͼ�ɰ󶨵Ķ�̬ί�У����ڽ�������Ϣ��FAuraAbilityInfo���㲥��UI���缼��ͼ�ꡢ��ȴʱ�䣩��


class UAbilitySystemComponent;
class UAttributeSet;
class AAuraPlayerController;
class AAuraPlayerState;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class UAbilityInfo;


USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController*PC,APlayerState*PS,UAbilitySystemComponent*ASC,UAttributeSet*AS):
	PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<APlayerController>PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState>PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr< UAbilitySystemComponent>AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr< UAttributeSet>AttributeSet = nullptr;

};


/**
 * 
 */

 // ����һ���̳���UObject���࣬������ΪUIС�����Ŀ��������ṩ��ҿ����������״̬������ϵͳ��������Լ�����Ϣ��
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);


	virtual void BroadCastInitialValues();

	virtual void BindCallbacksToDependencies();

	virtual void BroadcastAbilityInfo();

	UPROPERTY(BlueprintAssignable, Category = "GAS|Message")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo>AbilityInfo;

protected:

	// ֻ�����ԣ��洢��ҿ���������
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController>PlayerController;


	// ֻ�����ԣ��洢���״̬����
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState>PlayerState;


	// ֻ�����ԣ��洢����ϵͳ�������
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent;


	// ֻ�����ԣ��洢���Լ�����
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet>AttributeSet;

	// ֻ�����ԣ��洢��ҿ���������
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAuraPlayerController>AuraPlayerController;


	// ֻ�����ԣ��洢���״̬����
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAuraPlayerState>AuraPlayerState;


	// ֻ�����ԣ��洢����ϵͳ�������
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAuraAbilitySystemComponent>AuraAbilitySystemComponent;


	// ֻ�����ԣ��洢���Լ�����
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAuraAttributeSet>AuraAttributeSet;

	AAuraPlayerController* GetAuraPC();
	AAuraPlayerState* GetAuraPS();
	UAuraAbilitySystemComponent* GetAuraASC();
	UAuraAttributeSet* GetAuraAS();
};
