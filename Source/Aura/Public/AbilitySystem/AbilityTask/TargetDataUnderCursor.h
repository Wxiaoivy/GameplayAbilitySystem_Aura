// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystemComponent.h"
#include "TargetDataUnderCursor.generated.h"

//������Ŀ������׼����ʱ�㲥֪ͨ��ͨ��������ͼ�а��¼���
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/*
 * UTargetDataUnderCursor �࣬�̳��� UAbilityTask
 * ���ڻ�ȡ������µ�Ŀ�����ݣ��������ݷ��͵������� 
 */
UCLASS()
class AURA_API UTargetDataUnderCursor : public UAbilityTask
{
	GENERATED_BODY()

public:

	//DisplayName = "TargetDataUnderMouse"������ͼ����ʾ�ĺ������ơ�
	// HidePin = "OwningAbility"������ OwningAbility ��������Ϊ DefaultToSelf ���Զ����ò�������
	// DefaultToSelf = "OwningAbility"���� OwningAbility ����Ĭ������Ϊ���øú����� UGameplayAbility ʵ����
	// BlueprintInternalUseOnly����ʾ�ú�����Ҫ������ͼ�ڲ��߼���������ֱ�ӱ�¶���û���
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly))
	//static������һ����̬����������ͨ������ֱ�ӵ��ã�������Ҫ�������ʵ����
	//UGameplayAbility* OwningAbility��������ʾӵ�и� Ability Task �� UGameplayAbility ʵ����
	static UTargetDataUnderCursor* CreatTargetDataUnderMouse(UGameplayAbility* OwningAbility);


	// ����һ���ɰ󶨵Ķಥί�У�������Ŀ������׼����ʱ�㲥
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
	
private:
	virtual void Activate()override;
	void SendMouseCursorData();
};
