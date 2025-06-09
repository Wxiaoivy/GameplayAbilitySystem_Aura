// Copyright Ivy


#include "UI/WidgetController/SpellMenuWidgetController.h"
#include <../../../../../../../Source/Runtime/GameplayTags/Classes/GameplayTagContainer.h>
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"



void USpellMenuWidgetController::BroadCastInitialValues()
{
	BroadcastAbilityInfo();// ���ø��෽���㲥����������Ϣ
	OnSpellPointsChangedDelegateOnController.Broadcast(GetAuraPS()->GetSpellPoints());// ��Ϸ�տ�ʼʱ�㲥��ǰ��������
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{

	// �󶨵�����ϵͳ�����AbilityStatusChangedί��
	GetAuraASC()->AbilityStatusChanged.AddLambda
	([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
			{
				//��ѡ�е�����״̬�����仯ʱ����������UI��ť״̬, Ϊ�˱�֤״̬ͬ�������ԣ�ȷ���κο���Ӱ�찴ť״̬�ı仯���ᴥ��UI���£�ѡ������ʱ������״̬�仯ʱ��������仯ʱ��
				SelectedAbility.Status = StatusTag;
				bool bEnableSpendPointsButton = false;// ��ʼ�����ѵ�����ť״̬
				bool bEnableEquipButton = false;// ��ʼ��װ����ť״̬
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// ���ø�������ȷ����ť״̬
				SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton); // �㲥��ť״̬��UI

			}
			if (AbilityInfo)// ȷ��������Ϣ������Ч
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);// ��ȡָ����ǩ��������Ϣ
				Info.StatusTag = StatusTag;// ����Ϊ��ƥ���״̬��ǩ
				AbilityInfoDelegate.Broadcast(Info);// �㲥���º��������Ϣ
			}
		}
	);
	// �󶨵����״̬��OnSpellPointsChangedDelegateί��
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda
	(            [this](int32 SpellPoints)
		{
			OnSpellPointsChangedDelegateOnController.Broadcast(SpellPoints);// �����������仯ʱ���㲥��ֵ
			CurrentSpellPoints = SpellPoints;

			//���ã������������仯ʱ������������ť״̬��Ϊ�˱�֤״̬ͬ�������ԣ�ȷ���κο���Ӱ�찴ť״̬�ı仯���ᴥ��UI���£�ѡ������ʱ������״̬�仯ʱ��������仯ʱ��
			//��ʹ����״̬û�䣬�����仯���ܸı䰴ť������
			//���磺������0��Ϊ1�����ڿ��Ի��ѵ�����������

			bool bEnableSpendPointsButton = false;// ��ʼ�����ѵ�����ť״̬
			bool bEnableEquipButton = false;// ��ʼ��װ����ť״̬
			ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// ���ø�������ȷ����ť״̬
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton); // �㲥��ť״̬��UI
		}
	);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)// ��������ѡ�е��߼�
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();// ��ȡ��Ϸ��ǩ����
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints(); // ��ȡ��ǰ��������
	FGameplayTag StatusTag; // ���ڴ洢����״̬

	const bool bTagValid = AbilityTag.IsValid();// ���������ǩ�Ƿ���Ч
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);// ����Ƿ���"������"��ǩ
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);// ������ϵͳ��ȡ�������
	const bool bSpecValid = AbilitySpec != nullptr; // �����������Ƿ���Ч

	if (!bTagValid || bTagNone || !bSpecValid)// �����ǩ��Ч������������ǩ������Ч
	{
		StatusTag = GameplayTags.Abilities_Status_Lock;// ����Ϊ����״̬
	}
	else
	{
		StatusTag = GetAuraASC()->GetStatusTagFormSpec(*AbilitySpec);// �������������ȡ״̬��ǩ
	}
	
	SelectedAbility.Ability = AbilityTag;//ÿ��ѡ������ʱ��Ҫ�������øýṹ��,�Ѹ��º��ֵ�浽�ṹ���
	SelectedAbility.Status = StatusTag;//Ϊ�˱�֤״̬ͬ�������ԣ�ȷ���κο���Ӱ�찴ť״̬�ı仯���ᴥ��UI���£�ѡ������ʱ������״̬�仯ʱ��������仯ʱ��


	bool bEnableSpendPointsButton = false;// ��ʼ�����ѵ�����ť״̬
	bool bEnableEquipButton = false;// ��ʼ��װ����ť״̬
	ShouldEnableButtons(StatusTag, SpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// ���ø�������ȷ����ť״̬
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton); // �㲥��ť״̬��UI

}

// ��̬������������������״̬�ͷ�������������ť״̬
void FORCENOINLINE USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get(); // ��ȡ��Ϸ��ǩ
	bShouldEnableSpendPointsButton = false;// ��ʼ��Ϊfalse
	bShouldEnableEquipButton = false;
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) )// �������װ��״̬
	{
		bShouldEnableEquipButton = true; // װ����ť����
		if (SpellPoints > 0)// ����з�������
		{
			bShouldEnableSpendPointsButton = true;//���ѵ�����ť����
		}
	} 
	else if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))// ����ǽ���״̬
	{
		bShouldEnableEquipButton = true;// װ����ť����
		if (SpellPoints > 0)// ����з�������
		{
			bShouldEnableSpendPointsButton = true;//���ѵ�����ť����
		}
	}
	else if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))// ����Ƿ�������״̬
	{
		bShouldEnableEquipButton = false;// װ����ť������
		if (SpellPoints > 0)// ����з�������
		{
			bShouldEnableSpendPointsButton = true;//���ѵ�����ť����
		}
	}
   //���һ�����AbilityStatus == GameplayTags.Abilities_Status_Lock  ��������ֵ����false�� ����Ĭ��ֵ�����Բ���д������
}
