// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemBlueprintLibrary.h>
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayAbilitySpec.h>
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent()
{

}

FGameplayEffectContextHandle UAuraAbilitySystemComponent::MakeEffectContext() const
{
	// �����Զ���� FAuraGameplayEffectContext
	return FGameplayEffectContextHandle(new FAuraGameplayEffectContext());
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
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			//������ʵ����AbilitySpec����ӵ���ɫ�� AbilitySystemComponent �С�
			GiveAbility(AbilitySpec);

			
			bStartupAbilitiesGiven = true;
			AbilitiesGivenDelegate.Broadcast();
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
void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility>AbilityClass : StartupPassiveAbilities)
	{
		//FGameplayAbilitySpec: �� Gameplay Ability System (GAS) ��������������ʵ���Ľṹ�塣
		// AbilityClass: ��ǰѭ���е������ࡣ
		// 1 : ��ʾ�����ĵȼ���Level������������Ϊ 1��
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
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

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{

	FScopedAbilityListLock ActiveScopeLock(*this);//FScopedAbilityListLock �� UE �ṩ�� RAII����Դ��ȡ����ʼ������������ �̰߳�ȫ����ֹ�ڱ��������м����б��޸ģ��缼�ܱ��Ƴ�����ӣ���
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))//�����Ƿ���� if �飬ExecuteIfBound �����ȳ���ִ�д���if ֻ�Ǹ���ִ�н�������Ƿ�����֧����Ӱ��������ִ�С�
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFormSpec(const FGameplayAbilitySpec& AbilitySpec)
{

	if (AbilitySpec.Ability)
	{
		//�Ӽ��ܱ�ǩ����ȡ���� "Abilities" ����ı�ǩ���� "Abilities.Fireball"����
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;// ����ƥ�� "Abilities" ����ǩ��Tag
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFormSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	//�Ӷ�̬��ǩ����ȡ����󶨱�ǩ���� "Input.Q"��
	for (FGameplayTag InputTag : AbilitySpec.DynamicAbilityTags)
	{
		if (InputTag.MatchesTag((FGameplayTag::RequestGameplayTag(FName("InputTag")))))
		{
			return InputTag;// ����ƥ�� "Input" ����ǩ��Tag
		}
	}
	return FGameplayTag();
}


FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFormSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	//�Ӷ�̬��ǩ����ȡStatus��ǩ���� "Abilities.Status.Lock"��
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag((FGameplayTag::RequestGameplayTag(FName("Abilities.Status")))))
		{
			return StatusTag;// ����ƥ�� "Abilities.Status" ����ǩ��Tag
		}
	}
	return FGameplayTag();
}


FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);//����һ������������ȷ���ڱ��������б�ʱ���ᱻ�����߳��޸�  �����̰߳�ȫ��ʩ
	for (FGameplayAbilitySpec& AbilitySpec:GetActivatableAbilities())//�������пɼ���ļ���GetActivatableAbilities() ���ص��ǵ�ǰ �Ѿ�ͨ�� GiveAbility() 
		                                            //���� �� AbilitySystemComponent (ASC) ���� ���Ա����� �ļ����б�
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)//������ǰ���ܵı�ǩ�б�(AbilityTags),ÿ�����ܿ����ж����ǩ
		{
			if (Tag.MatchesTag(AbilityTag))//��鵱ǰ��ǩ�Ƿ��봫��ı�ǩƥ��
			{
				return &AbilitySpec;//����ҵ�ƥ���ǩ�ļ��ܣ����ظü��ܵ�ָ��
			}
		}
	}
	return nullptr;//���������м��ܶ�û�ҵ�ƥ��ģ����ؿ�ָ��
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusTagFormSpec(*Spec);
	}
	return FGameplayTag();
}


FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFormSpec(*Spec);
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)//������������ҵĵȼ����������������Level >= Info.LevelRequirement����
                                                                  //���Ҽ��� ��û�б����裨GetSpecFromAbilityTag(Info.AbilityTag) == nullptr�����Ż���� GiveAbility() �����¼��ܡ�
                                                                  //�������¼��ܾͻᱻ��ӵ� GetActivatableAbilities() ���б��֮��Ϳ��Լ������ˡ�
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());//��ȡ������Ϣ�����ʲ�(DataAsset) GetAbilityInfo()���Զ���ĸ������������ڻ�ȡ������Ϣ
	for (const FAuraAbilityInfo Info : AbilityInfo->AbilityInformation)//�������ж���ļ�����Ϣ,Info�ǰ������ܶ���Ľṹ��
	{
		if (!Info.AbilityTag.IsValid())continue;//������Ч��ǩ�ļ���
		if(Level<Info.LevelRequirement)continue;//�����ҵȼ������㼼�ܵȼ�Ҫ������	
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)//�������Ƿ�û���������(ʹ��ǰ�涨���GetSpecFromAbilityTag����)
	    {
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);//�����µļ��ܹ��,Info.Ability�Ǽ�����,1�Ǽ��ܵȼ�
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);//��������Ӷ�̬��ǩ"Eligible"(�ϸ�/����)
			GiveAbility(AbilitySpec);//�����ܸ������
			MarkAbilitySpecDirty(AbilitySpec);//��Ǽ��ܹ��Ϊ"��"��ȷ��ͬ��(������Ϸ�к���Ҫ),ǿ�Ʒ��������Ƹ��ͻ���
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible,1);//Client ��ʾ�������ֻ���� ���������ã�
			                                                                                                                 //Ȼ���� owning client��ӵ���������Ŀͻ��ˣ���ִ�С�
			                                                                                                                 //����������ִ�������������ֻ��ͨ�������ÿͻ���ִ�С�
			                                                                                                                 //��������ǵ��͵� "��������Ȩ���ͻ��˱���" ģʽ�������������߼����ͻ��˸������ UI ���Ӿ�Ч����
	    }
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag,AbilityLevel);
}

void UAuraAbilitySystemComponent::SeverSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))//ͨ��������ǩ���Ҷ�Ӧ��FGameplayAbilitySpec,AbilitySpec��������������ʱ��Ϣ���ȼ�����ǩ�ȣ�,����Ҳ�����Ӧ���ֱ�ӷ���
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			/*GetAvatarActor()����ȡӵ���������ϵͳ�Ľ�ɫ
				����ɫ�Ƿ�ʵ����UPlayerInterface�ӿ�
				ͨ���ӿڵ���AddToSpellPoints������Ϊ - 1��ʾ�۳�1��
				���ǵ��͵�Unreal�ӿڵ��÷�ʽ*/
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();//��ȡ��Ϸ��ǩ����
		FGameplayTag StatusTag = GetStatusTagFormSpec(*AbilitySpec);//����������л�ȡ��ǰ״̬��ǩ
		if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))//���������ǰ��"��������"״̬��	
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);//�Ƴ�"Eligible"��ǩ
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);//���"Unlocked"��ǩ
			StatusTag = GameplayTags.Abilities_Status_Unlocked;//���±���״̬��ǩ����

			//���ʾ���ѵ�����������������������
		}
		else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped)|| StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))//����"��װ��"��"�ѽ���"״̬������,��������Ѿ���"��װ��"��"�ѽ���"״̬��
		{
			AbilitySpec->Level += 1;// ֱ�����������ȼ� ���ʾ���ѵ�������������������
		}
		ClientUpdateAbilityStatus(AbilityTag, StatusTag, AbilitySpec->Level);//���ÿͻ���RPC֪ͨ״̬���� ����������������ǩ,��״̬��ǩ,�µȼ�.  ȷ���ͻ����������ͬ��
		MarkAbilitySpecDirty(*AbilitySpec);//֪ͨ����ϵͳ�ù�����޸�,��������ͬ������Ҫ��ȷ�����������
	}
	/*״̬ת���߼�
		�������ʵ��������״̬ת���߼���

		�ӷ����������ѽ�����
		�����ѵ�����"Eligible"״̬ʱ
		״̬�仯��Eligible �� Unlocked
		����Ӧ�ã�����������


		����������
		�����ѵ�����"Unlocked"��"Equipped"״̬ʱ
		ֱ�����������ȼ�
		����Ӧ�ã�ǿ����������


		����ͬ������:

		�ͻ��˷��𻨷ѵ�������
		������ִ�д˺�����
		�۳�����
		�޸�����״̬
		ͨ��RPC֪ͨ�ͻ���
		�ͻ����յ����º�ͬ��UI

		���Ҫ��:

		������Ȩ����
		���йؼ��������ڷ�����ִ��
		�ͻ���ֻ��ͨ��RPC����

		״̬������
		���ݵ�ǰ״̬������ͬ��Ϊ
		ʹ��GameplayTag����ʾ״̬

		����Ч�ʣ�
		ֻͬ����Ҫ�ı��
		ʹ��MarkDirty�Ż�ͬ��

		��չ�ԣ�
		ͨ���ӿ������״̬����
		��������µ�״̬����*/
}


void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)//bStartupAbilitiesGiven ��һ����ͨ�������Ǹ��Ʊ���������ˣ�����������Ϊ true �����Զ�ͬ�����ͻ��ˡ��ͻ�����Ҫ�� �Լ���ִ�л��� �ж������� bStartupAbilitiesGiven
	{
		//ActivateAbilities �����Ƶ��ͻ��� �� ���� OnRep_ActivateAbilities����ʱ�ͻ��˵� bStartupAbilitiesGiven ����Ĭ��ֵ false����Ϊδͬ�������ͻ�����Ҫ ������� ����� bStartupAbilitiesGiven = true
		bStartupAbilitiesGiven = true;//�����Լ�� (if (!bStartupAbilitiesGiven)) �Ƕ�����Ϸ��̵Ļƽ���ȷ���߼���׳�ԡ����������������� GAS �ĳ���ģʽ��������������Ҫ���״����ݾ�����֪ͨ�ĳ�����
		AbilitiesGivenDelegate.Broadcast();
	}
}



void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			SeverUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::SeverUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}

}

bool UAuraAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AuraGameplayAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraGameplayAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid()|| AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))//������Ϊ����ʱSpellPoint�����仯 ����OnSpellPointsChangedDelegate�����¼���
		                                                                                            //GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		                                                                                            //SelectedAbility�����Ability��ʼ����Ĭ��ֵ��Abilities_None��
																									//�����������û��ѡ���������AbilityTagҲ��Abilities_None,���Ի�������ʱĪ��������Description
		                                                                                            //���������if������
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}

	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::SeverEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag& PrevSlot = GetInputTagFormSpec(*AbilitySpec);//PrevSlot������֮ǰ�󶨵Ĳ�λ�����û�а󶨣������ǿձ�ǩ����
		const FGameplayTag& Status = GetStatusTagFormSpec(*AbilitySpec);//Status�����ܵ�ǰ��״̬��Equipped ��װ�� / Unlocked ��װ������


		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;//ֻ�� Equipped����װ������ Unlocked����װ���� �ļ��ܲ��ܱ����°󶨡�
		if (bStatusValid)
		{
			ClearAbilityOfSlot(Slot);//���Ŀ���λ��Slot���� �Ѿ��󶨵��������ܣ�����һ����λ�󶨶�����ܣ���
			ClearSlot(AbilitySpec);//�����ǰ���ܣ�AbilitySpec��֮ǰ�󶨵Ĳ�λ��PrevSlot��
			AbilitySpec->DynamicAbilityTags.AddTag(Slot);//���²�λ��Slot���󶨵���ǰ���ܡ�

			if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
			{
				//�������֮ǰ�� Unlocked����װ����δװ����������״̬��Ϊ Equipped����װ������
				AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Unlocked);
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			MarkAbilitySpecDirty(*AbilitySpec);//ȷ���������Ϳͻ���ͬ���������ݡ�
			ClientEquipAbility(AbilityTag, Status, Slot, PrevSlot);//ClientEquipAbility ��һ�� �ͻ��� RPC��֪ͨ�ͻ��˸��� UI���缼����ͼ�꣩��
		}
	}
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)//����Я����InputTag
{
	// 1. ��ȡ�ü��ܵ�ǰ�󶨵�����ۣ��� "Input.LMB"��
	const FGameplayTag Slot = GetInputTagFormSpec(*Spec);
	// 2. �� DynamicAbilityTags ���Ƴ������λ Tag
	Spec->DynamicAbilityTags.RemoveTag(Slot);
	// 3. ��Ǹü���Ϊ�������ݡ�����Ҫͬ�����ͻ��ˣ�
	MarkAbilitySpecDirty(*Spec);
}

void UAuraAbilitySystemComponent::ClearAbilityOfSlot(const FGameplayTag& Slot)
{
	// 1. ���������б���ֹ����ʱ���޸ģ�
	FScopedAbilityListLock ActiveScopeLock(*this);
	// 2. �������пɼ���ļ���
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		// 3. ������ܰ���Ŀ���λ��Slot�����������
		if (AbilityHasSlot(&Spec,Slot))
		{
			ClearSlot(&Spec);
		}
	}

	/*4. ��������ʾ��
		������ҽ� ������ �� ��������Input.LMB�� ���
		���� ClearAbilityOfSlot(Input.LMB)��
		�������м��ܣ��ҵ��� Input.LMB �ļ��ܣ������������
		���� ClearSlot �Ƴ� Input.LMB �� Tag��

		ClearSlot �ڲ���
		�� DynamicAbilityTags ���Ƴ� Input.LMB��
		��Ǽ���Ϊ�������ݡ���ͬ�����ͻ��ˡ�

		�����
		�������������ٴ�����������*/
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	// �������ܵ����ж�̬ Tag
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		// ����ҵ���ȫƥ��Ĳ�λ Tag������ true
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;// δ�ҵ�ƥ��� Tag
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

