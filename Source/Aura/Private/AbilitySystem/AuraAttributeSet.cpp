// Copyright Ivy


#include "AbilitySystem/AuraAttributeSet.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(100.f);
}
// UAuraAttributeSet���GetLifetimeReplicatedProps����ʵ��
// �����������ø����ͬ�������������Health���Ե���Ҫ���Ƶ������б���
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ʹ��DOREPLIFETIME_CONDITION_NOTIFY����ָ��Health������Ҫ���ƣ��������Ǵ���OnRep_Health����
	// COND_None��ʾû���ض���������Ҫ���㣬
	// REPNOTIFY_Always��ʾ���Ǵ���֪ͨ
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
 
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange( Attribute, NewValue);
	
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
			Props.SourceCharacter = Cast< ACharacter>(Props.SourceController->GetPawn());
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

	/*if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}*/
	
}

// UAuraAttributeSet���OnRep_Health����ʵ��
// �����������Health�����������ϱ�����ʱ������
// GAMEPLAYATTRIBUTE_REPNOTIFY�����ڴ������Ը��Ƶ�֪ͨ�߼�
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//GAMEPLAYATTRIBUTE_REPNOTIFY�����ר�����ڴ�����Ϸ�淨���ԣ�Gameplay Attributes������ʱ��֪ͨ��
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

