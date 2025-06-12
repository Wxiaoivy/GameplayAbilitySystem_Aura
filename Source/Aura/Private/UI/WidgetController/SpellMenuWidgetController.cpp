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
	//״̬ͬ�������ԣ�ȷ���κο���Ӱ�찴ť״̬�ı仯���ᴥ��UI����

	/*��ע����룺

		AbilityStatusChanged��������״̬�仯

		OnSpellPointsChanged������Դ�仯

		һ���Ա�֤�����۱仯�����ĸ�ϵͳ��UI���ܱ�����ȷ״̬

		��С�����£�ֻ�е�ѡ��������״̬���������仯ʱ�Ÿ��°�ť*/


	// �󶨵�����ϵͳ�����AbilityStatusChangedί��
	GetAuraASC()->AbilityStatusChanged.AddLambda
	([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 NewLevel)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))//��鵱ǰ�仯�������Ƿ�������ѡ�е�����(SelectedAbility)
			{
				//�����ѡ�е���������������״̬Ϊ�µ�״̬  Ϊ�˱�֤״̬ͬ�������ԣ�ȷ���κο���Ӱ�찴ť״̬�ı仯���ᴥ��UI���£�ѡ������ʱ������״̬�仯ʱ��������仯ʱ��
				SelectedAbility.Status = StatusTag;
				bool bEnableSpendPointsButton = false;// ��ʼ�����ѵ�����ť״̬
				bool bEnableEquipButton = false;// ��ʼ��װ����ť״̬
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// ���ø�������ȷ����ť״̬
				FString Description;//���������ַ��������������洢��ǰ�ȼ���������һ�ȼ�������
				FString NextLevelDescription;
				GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, Description, NextLevelDescription);//��ȡ��ǰ��������һ�ȼ��������ı�
				SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton,Description,NextLevelDescription); // �㲥��ť״̬���������ݸ�UI

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
			
			//���ã������������仯ʱ������������ť״̬��Ϊ�˱�֤״̬ͬ�������ԣ�ȷ���κο���Ӱ�찴ť״̬�ı仯���ᴥ��UI���£�ѡ������ʱ������״̬�仯ʱ��������仯ʱ��
			//��ʹ����״̬û�䣬�����仯���ܸı䰴ť������
			//���磺������0��Ϊ1�����ڿ��Ի��ѵ�����������
			CurrentSpellPoints = SpellPoints;
			bool bEnableSpendPointsButton = false;// ��ʼ�����ѵ�����ť״̬
			bool bEnableEquipButton = false;// ��ʼ��װ����ť״̬
			ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPointsButton, bEnableEquipButton);// ���ø�������ȷ����ť״̬
			FString Description;
			FString NextLevelDescription;
			GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton, Description, NextLevelDescription);// �㲥��ť״̬���������ݸ�UI
		}
	);

	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)// ��������ѡ�е��߼�
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}

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
	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bEnableEquipButton, Description, NextLevelDescription); // �㲥��ť״̬���������ݸ�UI

}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->SeverSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::GlobeDeselected()
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}
	

	SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Lock;

	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
	
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	WaitForEquipDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = true;

	FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection)return;
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType))return;
	
	GetAuraASC()->SeverEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(
	const FGameplayTag& AbilityTag,   // ��װ���ļ��ܣ��� "Ability.Fire.Firebolt"��
	const FGameplayTag& Status,       // ���ܵ���״̬���� "Status.Equipped"��
	const FGameplayTag& Slot,         // ���ܰ󶨵�����ۣ��� "Input.LMB"��
	const FGameplayTag& PreviousSlot  // ֮ǰ�󶨵Ĳ�λ������Ϊ�գ�       //����ʱ����������ڼ��ܲ˵��н�ĳ��������ק����ݼ���������������ʱ������
) 
{
	
	bWaitingForEquipSelection = false; // ��ǡ�����ѡ�����̡�����������رռ���ѡ�񵯴���

	//�ؼ����裺��վɰ� �� 2. �����°� �� 3. ֪ͨUI���¡�
	//��һ�ι㲥 (LastSlotInfo)����վɲ�λ��
	//�ڶ��ι㲥(Info)�������²�λ��
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;// ��Ǿɲ�λΪ����װ����(����Ҳ�̫����⣩
	LastSlotInfo.InputTag = PreviousSlot; // ��¼�ɲ�λ���� "Input.LMB"��
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None; // ��ռ���(����ͼ���Abilities_None�ķ�������Ϊ͸������
	AbilityInfoDelegate.Broadcast(LastSlotInfo);// ���֮ǰ�м��ܰ��� PreviousSlot����������ԭ�����˻���������������Ҫ ��������㲥 LastSlotInfo ����UI���������λ���ڿ��ˣ�������ʾĬ��ͼ�ꡱ��

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);// ���Ҽ�������
	Info.StatusTag = Status;// ����״̬���� "Equipped"��
	Info.InputTag = Slot; // ���°󶨵�����ۣ��� "Input.LMB"��
	AbilityInfoDelegate.Broadcast(Info);// ֪ͨUI����  ������װ���ļ�����Ϣ������ʾ������ͼ�굽��������λ�����㲥 Info ����UI���������λ���ڰ����¼��ܡ���

	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType); //֪ͨ����ϵͳ���缼�ܲ˵�UI������װ����������ɡ������Թرյȴ�״̬��ˢ�½��档

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
