// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "HAL/Platform.h" 
#include <../../../../../../../Source/Runtime/GameplayTags/Classes/GameplayTagContainer.h>
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"


// ����һ����̬�ಥί�У����Թ㲥����������ߣ�������ͼ��ʹ��
// ���ί������֪ͨ�������屻ѡ��ʱ������ť��״̬
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled);
/**
 * 
 */

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};


UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	virtual void BroadCastInitialValues()override;// ��д����Ĺ㲥��ʼֵ����	

	virtual void BindCallbacksToDependencies()override; // ��д�󶨻ص�����


	UPROPERTY(BlueprintAssignable)
	FOnStatChangedSignature OnSpellPointsChangedDelegateOnController;// ���������仯ί��

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;// ������ѡ��ί��


	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag); // ������ѡ��ʱ���õĺ�������������ͼ�е���

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

 private:
	 //��̬����������FORCENOINLINE���߱�������Ҫ�����������
	 // - AbilityStatus: ������ǰ״̬��ǩ
	 // - SpellPoints: ��ҵ�ǰӵ�еķ�������
	 // - bShouldEnableSpendPointsButton: ����������Ƿ����û��ѵ�����ť
	 // - bShouldEnableEquipButton: ����������Ƿ�����װ����ť
	 static void FORCENOINLINE  ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton);

	 FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None,FAuraGameplayTags::Get().Abilities_Status_Lock };//��ʼ������ṹ�壬ÿ��ѡ������ʱ��Ҫ�������øýṹ��
	 int32 CurrentSpellPoints = 0;
};
