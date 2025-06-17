// Copyright Ivy


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
//#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility(C++)"), true, true, FLinearColor::Yellow, 5.f);

	
	
	

	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& CombatSocket, bool bOverridePitch, float PitchOverride)
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
		FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), CombatSocket);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}


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

		Projectile->DamageEffectParams = MakeDamgeEffectParamsFormClassDefaults();//�����TargetActor��ʱ��Ϊnullptr��Ϊ����ֻ�������߼��� 
		                                                                                  //TargetActorӦ����EffectActor��BeginOverlap�ٴ����á�

		Projectile->FinishSpawning(SpawnTransform);
	}
}
