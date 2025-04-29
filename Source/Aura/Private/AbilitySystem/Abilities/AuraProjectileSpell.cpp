// Copyright Ivy


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
//#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility(C++)"), true, true, FLinearColor::Yellow, 5.f);

	
	
	

	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// ���ڷ������ˣ�Server������Ͷ���Projectile�������ڿͻ��ˣ�Client��ֱ�ӷ��أ���ִ�������߼���
	// ���Ƕ���������Ϸ�е�һ�ֳ������ģʽ��Ŀ����ȷ�� ����ͬ����Ȩ���ԣ�Authority�� �� �����ظ����ɡ�
	const bool bIsSever = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsSever)return;


	//���GetAvatarActorFromActorInfo()���ص�AActorʵ����ICombatInterface�ӿڣ���ôCombatInterface������Ч�ģ�����CombatInterfaceΪnullptr��
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		/*	Execute_GetCombatSocketLocation �� ��̬ȫ�ֺ��������������ǣ�
				����һ�� UObject* ������ͨ����ʵ���˸ýӿڵĶ��󣩡�
				���ö����Ƿ�ʵ���� ICombatInterface��
				���øö���� GetCombatSocketLocation() �� _Implementation �汾��*/
		FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo());
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;


		FTransform SpawnTransform;//�����������λ�ã���ICombatInterface��ʵ��
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, // ���������
			SpawnTransform,// ������ĳ�ʼ�任��λ�á���ת�����ţ� 
			GetOwningActorFromActorInfo(), // �������������,�����ǽ�ɫActor��SkeletonMeshComponent,�����߱�����AActor ��������������ߺ������߶��ǽ�ɫ�� 
			//��ʹ������һ��AActor���߼������������Ϊ�����˺����㡢�����߼�����ȻӦ�ù����ڽ�ɫ����Ϊ��ɫ�Ǹ��߲�ε��߼�ʵ�塣
			Cast<APawn>(GetOwningActorFromActorInfo()), // ������������ߣ�Instigator��      
			//(��Gameplay Ability System�У�GetOwningActorFromActorInfo()ͨ�����ڻ�ȡ��ǰ���ܵ�ʩ���ߣ�Caster����)

			ESpawnActorCollisionHandlingMethod::AlwaysSpawn // ��ײ����ʽ:��ʾ�����Ƿ�����ײ����������Actor����ʹ����λ�������������赲��������Ҳ�ᱻ���ɡ�
		);


		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();

		//����EffectContext����Ϣ
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors; 
		Actors.Add(Projectile); 
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult; 
		HitResult.Location = ProjectileTargetLocation; 
		EffectContextHandle.AddHitResult(HitResult);
		//����EffectContext����Ϣ

		const FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass,GetAbilityLevel(), EffectContextHandle);



		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

		for (auto& Pair : UAuraDamageGameplayAbility::DamageTypesMap)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(10/*GetAbilityLevel()*/);
			
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Pair.Key, ScaledDamage);//����ֵ���ض���GameplayTag�����洢
			//SetByCaller ��һ�ֶ�̬��ֵ���ݷ�ʽ�������� Gameplay Effect ִ��ʱ�������ֵ��������Ӳ�����������ʲ��
		    //��ĳ������Ч����EffectSpec����̬��һ���˺�ֵ��50��������˺�ֵ����������ʱ���޸ģ������ܼ��ܵȼ������Լӳɵ�Ӱ�죩��
		    //���磺
		    // ���������˺������� ����ǿ�ȣ�Spell Power�� Ӱ�죬����ʹ�� SetByCaller ��ʽ��̬���������˺���
		    // ���Ƽ��ܵ������������� ���Ƽӳɣ�Healing Bonus�� Ӱ�죬Ҳ���������ַ�ʽ��̬������
		}
		

		


		EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());
		EffectContextHandle.AddSourceObject(this);

		Projectile->DamageEffectSpecHandle = EffectSpecHandle;


		Projectile->FinishSpawning(SpawnTransform);
	}
}
