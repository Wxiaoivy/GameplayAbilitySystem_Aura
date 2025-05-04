// Copyright Ivy


#include "AbilitySystem/AuraAbilitySystemLibrary.h"


UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);

		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);

		}
	}
	return nullptr;

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
		ICombatInterface* CombatInterface = Cast<ICombatInterface>( ASC->GetAvatarActor() ) ;
		if (CombatInterface)
		{
          FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CombatInterface->GetPlayerLevel());
		  ASC->GiveAbility(AbilitySpec);
		}
		
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	//GameModeֻ�ڷ�������Ч ������ڿͻ��˷��ʻ᷵��Null,���������ڿͻ��˷��ʾͻᵼ��CharacterClassInfoҲ��Null
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameModeBase == nullptr)return nullptr;

	return AuraGameModeBase->CharacterClassInfo;
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
