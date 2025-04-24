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
	TObjectPtr<UBlackboardComponent>BlackboardComponent;*/ //这里可以不要这个变量 因为父类有Blackboard这个变量可以直接使用，Blackboard就是个UBlackboardComponent类型的变量。

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent>BehaviorTreeComponent;

};
