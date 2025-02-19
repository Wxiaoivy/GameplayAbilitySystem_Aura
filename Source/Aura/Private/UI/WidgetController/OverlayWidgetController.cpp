// Copyright Ivy


#include "UI/WidgetController/OverlayWidgetController.h"

void UOverlayWidgetController::BroadCastInitialValues()
{
	//Super::BroadCastInitialValues();//������Բ�ҪSuper  ��Ϊ�����ʵ�ֲ����ǿհ׵�

	const UAuraAttributeSet* AuraAttributeSet = CastChecked< UAuraAttributeSet>(AttributeSet);

	//�㲥������̬�ಥ����İ�����ͼ��ʵ�ֵģ�����Ѫ����������WBP�У� �Ȱ󶨺�㲥
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	//Super::BindCallbacksToDependencies();//������Բ�ҪSuper  ��Ϊ�����ʵ�ֲ����ǿհ׵�

	const UAuraAttributeSet* AuraAttributeSet = CastChecked< UAuraAttributeSet>(AttributeSet);

	//�������ڻ�ȡһ�����ڼ�����ǰ����ֵ���Ա仯��ί�С�����Lambda�󶨵�֮ǰ��ȡ��ί����
	//GetGameplayAttributeValueChangeDelegate������AbilitySystemComponent�Դ���һ���������������Ա仯ʱ���ô�����Զ�����
	//�ô�����һ���ಥ�����Ƕ�̬�����Բ���Ҫ��AddDynamic,��ֱ����AddUObject����AddLambda��
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetHealthAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	//�������ڻ�ȡһ�����ڼ����������ֵ���Ա仯��ί�С������ض��ĺ���MaxHealthChanged�󶨵�֮ǰ��ȡ��ί����
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetManaAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
	(AuraAttributeSet->GetMaxManaAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTag.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)//Lambda���ʽ���������ĳ����ĳ�Ա�����������ڷ������ﲶ����࣬
		                                              //������ǵ��ñ���ĺ���GetDataTableRowByTag�������Է�������дthis.
		{
			// ����AssetTags�����е����б�ǩ��
			for (const FGameplayTag& Tag : AssetTags)
			{
				//TODO:Broadcast the Tag to the WidgetController (TODO: ��WidgetController�㲥�����ǩ��������ʱδʵ�֣�������ʾ��)��


				//"A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
				//"Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
				
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				// ���д����鵱ǰ�������ı�ǩ��Tag���Ƿ�ƥ��MessageTag��
				// ���Tag��"Message"�����κκ�׺��չ����"Message.HealthPotion"����������Ϊ�档
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);//������Ϣ�������������ࣨ�㲥��
				}
			}
		}
	);
}

