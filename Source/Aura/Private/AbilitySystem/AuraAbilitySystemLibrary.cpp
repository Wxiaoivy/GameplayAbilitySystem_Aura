// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "UI/HUD/AuraHUD.h"

//����һ������ֵ(true/false)

//��������������
//WorldContextObject - ���ڻ�ȡ��Ϸ�����������
//OutWCParams - ����Ŀؼ�����(���ô��ݣ�����ֱ���޸�)
//OutAuraHUD - �����HUD����(���ô���)
bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))//���Ի�ȡ��ҿ�����(PlayerController)
	{
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD()); //����ҿ�������ȡHUD(PC->GetHUD())������ת��ΪAAuraHUD����
		if (OutAuraHUD)//����Ƿ�ɹ���ȡ��AuraHUD
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();//����ҿ�������ȡ���״̬(PlayerState)��ת��ΪAAuraPlayerState����
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();//�����״̬��ȡ����ϵͳ���(Ability System Component)
			UAttributeSet* AS = PS->GetAttributeSet(); //�����״̬��ȡ���Լ�(Attribute Set)

			//����ȡ���ĸ�������Ͷ���ֵ���������OutWCParams����Ӧ�ֶ�
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.AttributeSet = AS;
			OutWCParams.PlayerController = PC;
			OutWCParams.PlayerState = PS;

			return true;//���һ��˳��������true��ʾ�ɹ�
		}
	}
	return false;//����κ�һ��ʧ�ܣ����շ���false
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;//���������ֲ�������WCParams - �ؼ������ṹ��   AuraHUD - HUDָ�룬��ʼ��Ϊnullptr
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))//���õ�һ������MakeWidgetControllerParams��������,�������true(�ɹ�)������if��
	{
		return AuraHUD->GetOverlayWidgetController(WCParams);//����AuraHUD��GetOverlayWidgetController����������׼���õĲ���,���ػ�ȡ���ĸ��ǿؼ�
	}
	return nullptr;//���ʧ�ܣ����ؿ�ָ��

	//�����ⲿ�ֺ�����ע�����Ĵ���ʵ��һ�����ܣ�������һ���ġ�

	//if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	//{
	//	if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
	//	{
	//		AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
	//		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	//		UAttributeSet* AS = PS->GetAttributeSet();
	//		const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	//		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);

	//	}
	//}
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{

	FWidgetControllerParams WCParams;//���������ֲ�������WCParams - �ؼ������ṹ��   AuraHUD - HUDָ�룬��ʼ��Ϊnullptr
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))//���õ�һ������MakeWidgetControllerParams��������,�������true(�ɹ�)������if��
	{
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);//����AuraHUD��GetOverlayWidgetController����������׼���õĲ���,���ػ�ȡ���ĸ��ǿؼ�
	}
	return nullptr;//���ʧ�ܣ����ؿ�ָ��

}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;//���������ֲ�������WCParams - �ؼ������ṹ��   AuraHUD - HUDָ�룬��ʼ��Ϊnullptr
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))//���õ�һ������MakeWidgetControllerParams��������,�������true(�ɹ�)������if��
	{
		return AuraHUD->GetSpellMenuWidgetController(WCParams);//����AuraHUD��GetOverlayWidgetController����������׼���õĲ���,���ػ�ȡ���ĸ��ǿؼ�
	}
	return nullptr;//���ʧ�ܣ����ؿ�ָ��
}

//InitializeDefaultAttributesӦ���ڷ�������� ����Ӧ�ü��Ȩ�� HasAuthority()
const void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	AActor* AvatarActor = ASC->GetAvatarActor();

	FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassDefaultInfo.PrimaryAttribute, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());


	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttribute, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttribute, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());




}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr)return;

	for (auto AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		//FGameplayAbilitySpec: �� Gameplay Ability System (GAS) ��������������ʵ���Ľṹ�塣
		// AbilityClass: ��ǰѭ���е������ࡣ
		// 1 : ��ʾ�����ĵȼ���Level������������Ϊ 1��(CommonAbilities��Ϊ���������ܻ����������ܵȼ��仯���仯 ������Ĭ�Ͼ�д1��
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	for (auto AbilityClass : DefaultInfo.StartupAbilities)
	{
		 if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		 {
			 FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			 ASC->GiveAbility(AbilitySpec);
		 }
	}
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr)return 0;

	const FCharacterClassDefaultInfo Info = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}


UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	//GameModeֻ�ڷ�������Ч ������ڿͻ��˷��ʻ᷵��Null,���������ڿͻ��˷��ʾͻᵼ��CharacterClassInfoҲ��Null
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameModeBase == nullptr)return nullptr;

	return AuraGameModeBase->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameModeBase == nullptr)return nullptr;

	return AuraGameModeBase->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->IsBlockHit();
	}

	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->IsCriticalHit();
	}

	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->IsSuccessfulDebuff();
	}

	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetDebuffDamage();
	}

	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetDebuffDuration();
	}

	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetDebuffFrequency();
	}

	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AuraContext->GetDamageType().IsValid())
		{
			return *AuraContext->GetDamageType();
		}
	}

	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpuse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraContext->GetDeathImpuse();
	}

	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool InIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		 AuraContext->SetIsBlockedHit(InIsBlockedHit);
	}

}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool InIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetIsCriticalHit(InIsCriticalHit);
	}

}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool InIsSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetIsSuccessfulDebuff(InIsSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDuration)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffFrequency)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpuse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpuse)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraContext->SetDeathImpuse(InImpuse);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
	const UObject* WorldContextObject,  // ���ڻ�ȡWorld�����ĵĶ���
	TArray<AActor*>& OutOverlapingActors,  // ����������洢����������Actor����
	const TArray<AActor*>& ActorsToIgnore,  // ��Ҫ���Ե�Actor�б�
	float Radius,  // �����뾶
	const FVector& SphereOrigin)  // �������ԭ��λ��
{
	FCollisionQueryParams SphereParams;//FCollisionQueryParams SphereParams ��Ҫ���� ������ײ���ľ�ϸ��Ϊ
	SphereParams.AddIgnoredActors(ActorsToIgnore);//SphereParams�ĺ������ã�AddIgnoredActors  ���˲���Ҫ����Actor

	if (const UWorld*World=GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))//ͨ�������WorldContextObject��ȡ��ǰ��World���������ȡʧ���򷵻�nullptr��
	{
		TArray<FOverlapResult>Overlaps;//�������ڴ洢�ص�����������顣

		World->OverlapMultiByObjectType(//ִ�������ص���⣬���ָ���뾶��Χ�������������ص��Ķ���
			Overlaps,  // �洢�����������
			SphereOrigin,  // ��������λ��
			FQuat::Identity,  // ������ת(ʹ������ת)
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),  // ������ж�̬����
			FCollisionShape::MakeSphere(Radius),  // ����������ײ��״
			SphereParams);  // ��ײ��ѯ����


	    for (FOverlapResult&Overlap: Overlaps)//���������ص��������
	    {
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))//���Actorʵ����ս���ӿڲ����Ǵ��ģ�
				//��·��ֵ�� && �� || ��Ĭ����Ϊ����Ҫ��;������������ָ�롢δʵ�ֽӿڵĵ��ã����Ż����ܣ���������Ҫ�ļ��㣩��� A Ϊfalse���������ʽһ���� false��B ���ᱻִ�С�ֻ�� A Ϊ true ʱ���Ż���� B��
			{	
					OutOverlapingActors.AddUnique(Overlap.GetActor());//����Actor��ӵ����������(ȷ�����ظ����)
			}
	    }
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool BothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool BothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	if (BothArePlayers|| BothAreEnemies)
	{
		return false;
	}
	else
	{
		return true;
	}
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpuse(EffectContextHandle, DamageEffectParams.DeathImpuse);

	FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->
	MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}

