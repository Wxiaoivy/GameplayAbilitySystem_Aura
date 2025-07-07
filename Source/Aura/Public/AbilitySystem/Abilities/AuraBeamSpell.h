// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
 // ����һ������ UAuraDamageGameplayAbility ���Զ��弼���࣬����ʵ��"����"���ͼ���
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	// �洢�����������Ϣ
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);
	// �洢ӵ������ҿ�����
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

protected:
	// �������λ�ã��ɱ���ͼ��д��
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;

	//������е�Actor��ʹ��TObjectPtr��ǿָ�룩
	// ע�⣺��Ϊ��UPROPERTY()����������ǿ���ã�����ֹGC���ո�Actor
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor>MouseHitActor;

	// ӵ������ҿ�������ʹ��TObjectPtr��ǿָ�룩
	// ��Ϊ��UPROPERTY()��������ǿ���ã�����ֹGC����PlayerController
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController>OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter>OwnerCharacter;

};
