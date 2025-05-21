// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "WaitCooldownChange.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);//����һ�����Զ�̬�󶨵Ķಥί�У��������ط������� float TimeRemaining���ص�ʱ���ݵĲ�����ʣ����ȴʱ�䣩��

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase//UBlueprintAsyncActionBase��UE�ṩ���첽������࣬��������̨����
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)//UPROPERTY(BlueprintAssignable)�������ί�п�������ͼ�а󶨡�
	FCooldownChangeSignature CooldownStart;// ��ȴ��ʼʱ����

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd; // ��ȴ����ʱ����

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))//Ϊʲô�þ�̬��������Ϊ�첽������Ҫ�ȴ���ʵ�����ٿ�ʼ��������̬��������ֱ�ӵ��ã�������ͼʹ�á�
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbillitySystemComponent, const FGameplayTag& InCooldownTag);
	UFUNCTION(BlueprintCallable)
	void EndTask();//���ã�ֹͣ������ȴ�¼����������ȡ���˼��ܣ���Ϊʲô��Ҫ�����������ֶ���ֹ����ʹ���ܽ����ˣ�ί����Ȼ����������ܵ����ڴ�й©��

protected:

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>ASC;

	FGameplayTag CooldownTag;

	void CooldownTagChange(const FGameplayTag InCooldownTag, int32 NewCount);//����ȴ��ǩ�����仯ʱ���ã�������ȴ��������
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle); //������Ч����GameplayEffect������ӵ���ɫʱ���á�

	/*Ϊʲô��Ҫ�������ص�������
		��Ϊ��ȴ�����ɼ���Ч��������Ҳ�����ɱ�ǩֱ�ӿ��ƣ�����Ҫ���������¼���*/


};
