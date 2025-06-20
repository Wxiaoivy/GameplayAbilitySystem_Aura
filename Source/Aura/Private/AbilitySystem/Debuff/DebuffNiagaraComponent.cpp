// Copyright Ivy


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Interaction/CombatInterface.h"
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemBlueprintLibrary.h>
#include <../../../../../../../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemComponent.h>

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;// �ڹ��캯�������� bAutoActivate Ϊ false����ʾ��� Niagara ����Ч�������Զ����ţ���Ҫ������Ϸ�߼��ֶ����
}

void UDebuffNiagaraComponent::BeginPlay()
{
	/*�����������Ϸ��ʼʱ���ã����ø����¼�����
		���ȳ��Ի�ȡӵ���ߵ�����ϵͳ���(ASC)
		���ASC���ڣ�����ע����ض���Ϸ��ǩ(DebuffTag)�仯�Ļص�
		���ASC�������ڵ�ӵ��ս���ӿڣ�����һ��lambda��������ASCע�������ӻص�
		���ע�������¼��Ļص�*/
	
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());// ���Ի�ȡӵ���ߵ�ս���ӿ�
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		// ����ɹ���ȡASC��ע���DebuffTag��ǩ�仯�Ļص�
		//������ȷָ����Ҫ�������� DebuffTag ����ض���ǩ�ı仯
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if (CombatInterface)
	{
		// �����ǰû��ASC��ӵ��ս���ӿڣ�����һ��ί�У���ASCע��ʱ����ӱ�ǩ�ص�
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda//�����ò���AddWeakLambdaҪ����,����ί�У���������֧�� ���ĳ������� "����ί�� + �����������ڿɿ�"����� AddLambda �����ʡ�
		(        [this](UAbilitySystemComponent* InASC)        //��ΪCombatInterface ͨ���ǽ�ɫ�������һ���֣���ɫ����ʱί��Ҳ�ᱻ�ͷš�
			{
				InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
			}
		);
	}
	if (CombatInterface)
	{
		// ���ӵ��ս���ӿڣ�ע�������¼��Ļص�
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}
//CallbackTag��ʵ�ʴ����仯�ı�ǩ��������ע��ʱָ���� DebuffTag�� NewCount���ñ�ǩ��ǰ��ASC�ϵļ���,��Ȼ��������û�ã����Ǳ�ʾ�˵����Ǽ���ĸ�Tag�ı仯 �����CallbackTag=DebuffTag
void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		Activate();// �����ǩ��������0����������Ч��
	}
	else
	{
		Deactivate();// ����ͣ������Ч��
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();// ��ӵ��������ʱͣ������Ч��
}
