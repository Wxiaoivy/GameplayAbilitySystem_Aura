// Copyright Ivy


#include "UI/WidgetController/OverlayWidgetController.h"

void UOverlayWidgetController::BroadCastInitialValues()
{
	Super::BroadCastInitialValues();//������Բ�ҪSuper  ��Ϊ�����ʵ�ֲ����ǿհ׵�

	const UAuraAttributeSet* AuraAttributeSet = CastChecked< UAuraAttributeSet>(AttributeSet);

	//�㲥������̬�ಥ����İ�����ͼ��ʵ�ֵģ�����Ѫ����������WBP�У� �Ȱ󶨺�㲥
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();//������Բ�ҪSuper  ��Ϊ�����ʵ�ֲ����ǿհ׵�

	const UAuraAttributeSet* AuraAttributeSet = CastChecked< UAuraAttributeSet>(AttributeSet);

	//�������ڻ�ȡһ�����ڼ�����ǰ����ֵ���Ա仯��ί�С������ض��ĺ���HealthChanged�󶨵�֮ǰ��ȡ��ί����
	//GetGameplayAttributeValueChangeDelegate������AbilitySystemComponent�Դ���һ��������
	//�ô�����һ���ಥ�����Ƕ�̬�����Բ���Ҫ��AddDynamic,��ֱ����AddUObject��
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	//�������ڻ�ȡһ�����ڼ����������ֵ���Ա仯��ί�С������ض��ĺ���MaxHealthChanged�󶨵�֮ǰ��ȡ��ί����
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTag.AddLambda(
		[](const FGameplayTagContainer& AssetTags)
		{
			// ����AssetTags�����е����б�ǩ��
			for (const FGameplayTag& Tag : AssetTags)
			{
				//TODO:Broadcast the Tag to the WidgetController (TODO: ��WidgetController�㲥�����ǩ��������ʱδʵ�֣�������ʾ��)��

				 // ����һ����Ϣ�ַ�������ʽΪ"GE Tag:��ǩ����"��
				const FString Msg = FString::Printf(TEXT("GE Tag:%s"), *Tag.ToString());
				// ʹ����Ϸ���棨GEngine������Ļ�����һ��������Ϣ��
				// -1��ʾ��Ϣ�����������Ļ����ʾ��8.f����Ϣ��ʾ��ʱ�䳤�ȣ��룩��FColor::Blue����Ϣ����ɫ��Msg����Ϣ���ݡ�
				GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);

			}
		}
	);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data)const
{
	OnHealthChanged.Broadcast(Data.NewValue); 
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data)const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}

