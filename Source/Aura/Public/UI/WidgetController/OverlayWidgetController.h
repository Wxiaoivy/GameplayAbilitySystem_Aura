// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "OverlayWidgetController.generated.h"


class UAuraUserWidget;
class UAbilityInfo;
class UAuraAbilitySystemComponent;
USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;

};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);//����һ����ͼ�ɰ󶨵Ķ�̬ί�У����ڽ�������Ϣ��FAuraAbilityInfo���㲥��UI���缼��ͼ�ꡢ��ȴʱ�䣩��

/**
 * 
 */



UCLASS(BlueprintType,Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadCastInitialValues()override;

	virtual void BindCallbacksToDependencies()override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Message")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Message")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnStatChangedSignature OnPlayerLevelChangedDelegate;

protected:

	//FOnAttributeChangeData ��һ���Զ���Ľṹ����࣬ר��������ڴ������Ա仯����Ϣ��
	//�����������Ա�����������ֵ��OldValue������ֵ��NewValue�����������ƣ�AttributeName���ȣ�������ϸ�������Ա仯�ĸ������档

	//������ HealthChanged �����У�FOnAttributeChangeData ����Ϊ��������ġ�����ζ����ĳ���ط������������Ա仯�Ĵ����㣩��
	//��һ������Ƭ�δ�����һ�� FOnAttributeChangeData ʵ�������������صı仯��Ϣ��Ȼ������� HealthChanged �����������ʵ����Ϊ�������ݡ�
	//void HealthChanged(const FOnAttributeChangeData& Data)const;

	//void MaxHealthChanged(const FOnAttributeChangeData& Data)const;

	//void ManaChanged(const FOnAttributeChangeData& Data)const;

	//void MaxManaChanged(const FOnAttributeChangeData& Data)const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable>MessageWidgetDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo>AbilityInfo;


	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC);


	//�����T��ʾ����ʲô���͵�DataTable;�Ǹ�ģ�庯�������Դ��κ����͵�DataTable��ȡ��Ϣ��
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	void OnXPChange(int32 NewXP)const;

}; 

template<typename T>
inline T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	//ͨ����ͨ��RowName����ȡ���У�Row������Ϣ������������DataTable���RowNameֱ��д����TagName���Կ���ֱ������Tag.GetTagName()�����RowName.

	T* Row = DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));//TEXT("")ûʲô���� ֻ��Ϊ�˴ղ����ġ�
	return Row;
}
