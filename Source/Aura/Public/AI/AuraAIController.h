// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AuraAIController.generated.h"

/**
 * 
 */

class UBehaviorTreeComponent;
UCLASS()
class AURA_API AAuraAIController : public AAIController
{
	GENERATED_BODY()


public:
	AAuraAIController();


protected:

	/*UPROPERTY()
	TObjectPtr<UBlackboardComponent>BlackboardComponent;*/ //������Բ�Ҫ������� ��Ϊ������Blackboard�����������ֱ��ʹ�ã�Blackboard���Ǹ�UBlackboardComponent���͵ı�����

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent>BehaviorTreeComponent;

};
