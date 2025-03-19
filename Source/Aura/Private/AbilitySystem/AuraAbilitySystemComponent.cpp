// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemComponent.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent()
{

}

//���������AuraCharacterBase��ͬ����������
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	// ����һ�� for ѭ�������ڱ��� StartupAbilities �����е�ÿһ�������ࡣ
	// TSubclassOf<UGameplayAbility>AbilityClass: ÿ��ѭ��ʱ��AbilityClass ����������е�һ�������ࡣ
	// StartupAbilities: �Ǵ�����������顣
	for (const TSubclassOf<UGameplayAbility>AbilityClass : StartupAbilities)
	{
		//FGameplayAbilitySpec: �� Gameplay Ability System (GAS) ��������������ʵ���Ľṹ�塣
		// AbilityClass: ��ǰѭ���е������ࡣ
		// 1 : ��ʾ�����ĵȼ���Level������������Ϊ 1��
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		//��� AbilitySpec.Ability �Ƿ��� UAuraGameplayAbility ���͵�ʵ����
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast< UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//����ǣ���һ�������ǩ��StartupInputTag����ӵ�����ʵ���� DynamicAbilityTags ��
			//AbilitySpec.DynamicAbilityTags: ������ʵ���Ķ�̬��ǩ���ϣ����ڴ洢��������صı�ǩ��
			//AuraGameplayAbility->StartupInputTag: �� UAuraGameplayAbility �ж���������ǩ����ʾ��������Ӧ�����루�簴������
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupInputTag);
			//������ʵ����AbilitySpec����ӵ���ɫ�� AbilitySystemComponent �С�
			GiveAbility(AbilitySpec);
		}
		                    

	   //������ʵ����AbilitySpec����ӵ���ɫ�� AbilitySystemComponent �С�
	   //����:
	   //GiveAbility: �� AbilitySystemComponent ��һ�����������ڽ�������ӵ���ɫ�С�
       //AbilitySpec : ����һ������������ʵ����
	   //GiveAbility(AbilitySpec);//�����������������һ�д���дconst������
		
	
	   //GiveAbilityAndActivateOnce: �� AbilitySystemComponent ��һ����������������������������
		//GiveAbilityAndActivateOnce(AbilitySpec);//���������������������ڶ��д���дconst������


		//GiveAbilityAndActivateOnce �����������������������Ҫ�����������ֻʹ�� GiveAbility��

		//���������Ҫ�ض��ļ����������簴��������������Ҫ���� GiveAbilityAndActivateOnce��
	}
}

//���������AuraPlayerController��ͬ����������
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);

			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

//���������AuraPlayerController��ͬ����������
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);

		}
	}

}

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//OnGameplayEffectAppliedDelegateToSelf�Ǹ����һ������ʵ��������󶨻ص��������˴���
	//OnGameplayEffectAppliedDelegateToSelfÿ��һ����ϷЧ����Game Effect�����GE����Ӧ�õ�������ʱ�������Ǽ�ʱЧ�����ǳ���Ч����������������ô˷�������
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

	//ͨ��FAuraGameplayTags::Get()��������˸����Ψһ����������GameplayTags��
	//ͨ��ʵ�����Է���Attribute_Secondary_Armor��Ա����
	//const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Orange, FString::Printf(TEXT("Tag:%s"), *GameplayTags.Attribute_Secondary_Armor.ToString()));
	// FGameplayTag Tag = FAuraGameplayTags::Get().Attribute_Primary_Intelligence;//���Կ�����C++�з�����Щnative tags
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{

	// ����һ����Ϸ�淨��ǩ������TagContainer�������ڴ洢��Ч���淶����ȡ�ı�ǩ��
	FGameplayTagContainer TagContainer;
	// ��Ч���淶�л�ȡ���е��ʲ���ǩ���������Ǵ洢��TagContainer�С�
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTag.Broadcast(TagContainer);//�㲥����WidgetController���������ݣ�
}

