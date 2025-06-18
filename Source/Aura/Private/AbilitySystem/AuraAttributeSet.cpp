// Copyright Ivy


#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerController.h"
#include "Interaction/PlayerInterface.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();


	//�� C++ �У����������������Ǻ����ĵ�ַ��������ָ�룩���������������ţ��� GetStrengthAttribute()����ʾ�Ժ����ĵ��á�
	// ��ˣ��� TagsToAttributes.Add �У�GetStrengthAttribute ������������Ϊ������Ҫ���Ǻ����ĵ�ַ������ָ�룩�������ǵ��ú����Ľ����


	/*Primary Attributes*/
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Strength, GetStrengthAttribute);//���GetStrengthAttribute��ͨ��ACCESSORS���ʵĺ������Ǿ�̬������
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Vigor, GetVigorAttribute);


	/*Secondary Attributes*/
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_MaxMana, GetMaxManaAttribute);


	/*Resistance Attributes*/
	TagsToAttributes.Add(GameplayTags.Attribute_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Resistance_Physical, GetPhysicalResistanceAttribute);


}




// UAuraAttributeSet���GetLifetimeReplicatedProps����ʵ��
// �����������ø����ͬ�������������Health���Ե���Ҫ���Ƶ������б���
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ʹ��DOREPLIFETIME_CONDITION_NOTIFY����ָ��Health������Ҫ���ƣ��������Ǵ���OnRep_Health����
	// COND_None��ʾû���ض���������Ҫ���㣬
	// REPNOTIFY_Always��ʾ���Ǵ���֪ͨ

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
 
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);




}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange( Attribute, NewValue);
	

	//�����NewValue��CurrentValue,������GE���޸�BaseValue��,BaseValue���޸�ʱ�����¼���������Եľۺ����еĸ���Mod,����޸�CurrentValue 
	//ֻǯ����CurrentValueû��ǯ��BaseValue GetValue��ȡ����CurrentValue ����ʵ����BaseValueһֱ�����
	/*if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}*/
}

void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	//�����NewValue��BaseValue


	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}



//FGameplayEffectModCallbackData��������¹ؼ���Ϣ��

//���޸ĵ����ԣ�ָ���ĸ����Ա�Gameplay Effect���޸ġ�
//�޸�ǰ���ֵ�����������޸�ǰ��Ļ�ֵ��BaseValue���͵�ǰֵ��CurrentValue�����������ڿ������˽�Gameplay Effect�����Եľ���Ӱ�졣
//Gameplay Effect�����ã��ṩ�Դ����˴��޸ĵ�Gameplay Effect�����ã��Ա㿪�����ڻص������и�����Ҫ��ȡ������ڸ�Ч������Ϣ��
void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)const
{
	// ��Data�л�ȡ��ϷЧ���������ģ�Context������������İ����˹�����ϷЧ����α�ִ�е���Ϣ��
	
	Props.EffectContextHandle = Data.EffectSpec.GetContext();

	// ���������л�ȡ������ϷЧ����ԭʼ�����ߵ�����ϵͳ�����ASC����
	// ASC�ǹ����ɫ���������Ժ���ϷЧ���ĺ��������
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	// ���SourceASC�Ƿ���Ч���Լ�����AbilityActorInfo��AvatarActor�Ƿ���Ч��
	// AbilityActorInfo�����˹�������ϵͳ�����������ɫ����Ϣ��AvatarActor�������ɫ��ʵ����Ϸ����
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		// ��AbilityActorInfo�л�ȡ��ɫ����Ϸ����Actor��
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();

		// ���Դ�AbilityActorInfo�л�ȡ��ɫ�Ŀ�������Controller����
		// �������Ǵ����������ͽ�ɫ�����������
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		// ���û���ҵ������������ҵ��˽�ɫ����Ϸ������ô���Դ���Ϸ�����л�ȡ��������
		// ��ͨ�������ڽ�ɫ����AI���ƶ�������ҿ��Ƶ�����¡�
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{

			// ���SourceAvatarActor���Ա�ת��ΪAPawn���ͣ�APawn��AActor��һ�����࣬������Ա����ƵĽ�ɫ����
			// ��ô���������ȡ��������
			if (const APawn* Pawn = Cast <APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();

			}

		}

		// ����ҵ��˿���������ô���Դӿ������л�ȡ�����ƵĽ�ɫ��Pawn����
		// Ȼ�����Pawnת��ΪACharacter���ͣ�ACharacter��APawn��һ�����࣬��������ض���Ϸ�߼��Ľ�ɫ����
		// ע�⣺�����ת������ʧ�ܣ����Pawn����ACharacter���͵Ļ�����������û�д������������
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	// ���Data�е�Ŀ����Ϣ�Ƿ���Ч���Լ�Ŀ���AvatarActor�Ƿ���Ч��
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		// ��Ŀ�������ϵͳ��Ϣ�л�ȡĿ�����Ϸ����Actor����
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();

		// ���Դ�Ŀ�������ϵͳ��Ϣ�л�ȡĿ��Ŀ�������
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();

		// ���Խ�Ŀ�����Ϸ����ת��ΪACharacter���͡�
		// ע�⣺�����ת������ʧ�ܣ���������û�д������������
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);

		// ʹ��UAbilitySystemBlueprintLibrary�ľ�̬����GetAbilitySystemComponent����ȡĿ�����Ϸ���������ϵͳ�����
		// ���������Unreal Engine�����ڻ�ȡASC�ĳ��÷���
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

// �����������ϷЧ��ִ�к󱻵��ã��������Ǵ�����ϷЧ��ִ�к���߼���
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);


	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	//if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	//{
	//	SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	//}

	/*if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	*/
	//��鱻�޸ĵ������Ƿ���"IncomingDamage"(�����˺�)
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	//Data.EvaluatedData.Attribute == GetIncomingXPAttribute() ��ʾ��GAS �Ѿ�����˶� IncomingXP ���޸ģ��������ڴ�������߼���GetIncomingXP() ��ȡ���� �Ѿ��� GameplayEffect ���º��ֵ��
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::HandleIncomingXP(FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();//��ȡ��ǰ��XPֵ

	SetIncomingXP(0);//������XP��������Ϊ0(Ϊ��һ�ε�����XP��׼��)

	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 NumLevelUps = NewLevel - CurrentLevel;

		if (NumLevelUps > 0)
		{
			const int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, NewLevel);//�Ҿ�������Ӧ���Ǵ���NewLevel��
			const int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel);

			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);
			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

			bTopOffHealth = true; //����ɫ����ʱ��bTop0ffHealth��bTop0ffMana������Ϊtrue���������߼������ã�,��ֻ����MaxHealth��MaxMana���Է����仯ʱ���Ż����PostAttributeChange����
			bTopOffMana = true;

			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}

		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}



void UAuraAttributeSet::HandleIncomingDamage(FEffectProperties& Props)
{
	
	const float LocalIncomingDamage = GetIncomingDamage();//��ȡ��ǰ���˺�ֵ

	SetIncomingDamage(0);//�������˺���������Ϊ0(Ϊ��һ���˺���׼��)

	if (LocalIncomingDamage > 0)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;//�����µ�����ֵ(��ǰ����ֵ��ȥ�˺�)

		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));//ʹ��FMath::Clampȷ������ֵ��0�����ֵ֮��

		const bool bFetal = NewHealth <= 0;//���������ֵ�Ƿ�С�ڵ���0(���Ƿ�����)

		if (bFetal)
		{
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
			if (CombatInterface)
			{
				CombatInterface->die();
			}
			SendXPEvent(Props);
		}
		else
		{
			//���û�����������Լ������"Effects_HitReact"��ǩ�ļ���
			//��ᴥ���ܻ������������ܻ���Ӧ�߼�
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			//Data�������ṩ��ԭʼ�ص����ݣ���������GameplayEffectִ�е�ԭʼ��Ϣ
			// Props�ǿ������Զ���Ľṹ�壬ͨ��SetEffectProperties������Data����ȡ����֯�ɸ����õ���ʽ

			// ��������£�Props���ܰ�����

			//Ŀ��ASC(AbilitySystemComponent)

			//��ԴASC

			//Ŀ��Actor

			//��ԴActor

			//Ч�����(EffectSpec)��
		}
		bool bIsBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		bool bIsCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);


		if (Props.SourceCharacter != Props.TargetCharacter)
		{

			if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceCharacter->GetController()))
			{
				//��������ǹ�����(SourceCharacter)������ҹ�������ʱ���������ڹ���Ŀ��������ʾ��ɵ��˺�����
				PC->ShowDamageNumber(LocalIncomingDamage, Props.TargetCharacter, bIsBlockedHit, bIsCriticalHit);
				return;
			}
			if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetCharacter->GetController()))
			{
				//����������ܻ���(TargetCharacter)������ұ�����ʱ�����������Լ���ɫ������ʾ�ܵ����˺�����
				PC->ShowDamageNumber(LocalIncomingDamage, Props.TargetCharacter, bIsBlockedHit, bIsCriticalHit);
			}
		}
		if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			Debuff(Props);
		}
	}
}

void UAuraAttributeSet::Debuff(FEffectProperties& Props)
{

}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	//�ں����ڲ�������⵽��Ӧ�����Ա仯�Ҳ���ֵΪtrueʱ���Ż����SetHealth()��SetMana()��������ֵ
	//����ζ���������ʱMaxHealth/MaxManaû��ʵ�ʱ仯���������ֵû����������PostAttributeChange���ᱻ���ã�Ѫ/��Ҳ�Ͳ��ᱻ������	 
	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
    {
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
    }
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	
	if (Props.TargetCharacter->Implements<UCombatInterface>())//���Ŀ���ɫ(Props.TargetCharacter)�Ƿ�ʵ����ICombatInterface�ӿ�,ICombatInterface������2���������Ի�õȼ���ְҵ
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);//��ȡĿ��ĵȼ�(TargetLevel)��ְҵ(TargetClass)��Щ��Ϣ�����ڼ���XPReward
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);//��ȡĿ��ĵȼ�(TargetLevel)��ְҵ(TargetClass)��Щ��Ϣ�����ڼ���XPReward
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);//����һ����̬����GetXPRewardForClassAndLevel�����㾭��ֵ

		//����һ��FGameplayEventData�ṹ��Payload(�¼�����)�����¼���ǩΪ"Attribute_Meta_IncomingXP"(��ʾ"��þ���ֵ"�¼�)�����¼���ֵ(EventMagnitude)Ϊ������ľ���ֵ����
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags().Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attribute_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;

		//���Payload������ݴ��ݸ���ͼGA_ListenForEventִ�к���SetByCallerMagenitude��ApplyEffectSpecToSelf���߼�
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attribute_Meta_IncomingXP, Payload);
		//���������¼����͸�Դ��ɫ(Props.SourceCharacter������ɱ��)����¼��ᱻ��ɫ��AbilitySystemComponent���ղ�����
	}
}


// UAuraAttributeSet���OnRep_Health����ʵ��
// �����������Health�����������ϱ�����ʱ������
// GAMEPLAYATTRIBUTE_REPNOTIFY�����ڴ������Ը��Ƶ�֪ͨ�߼�
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//GAMEPLAYATTRIBUTE_REPNOTIFY�����ר�����ڴ�����Ϸ�淨���ԣ�Gameplay Attributes������ʱ��֪ͨ
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}