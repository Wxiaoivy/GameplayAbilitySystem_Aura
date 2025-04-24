// Copyright Ivy


#include "AI/BTService_FindNearestPlayer.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include <../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BTFunctionLibrary.h>

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();

	FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*>ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn,TargetTag, ActorsWithTag);


	float ClosestDistance = TNumericLimits<float>::Max();//TNumericLimits 的静态方法，返回 float 类型能表示的最大正数值,
	                                                     //通常用于记录算法中的“最近距离”（如寻找最近敌人、最近路径点等）。
														 // 初始化为最大值，确保后续比较时任何实际距离都会比它小，从而被正确更新。

	AActor* ClosestActor = nullptr;

	for (AActor* Actor : ActorsWithTag)
	{
		//GEngine->AddOnScreenDebugMessage(2, .5f, FColor::Green, *Actor->GetName());

		if (IsValid(Actor) && IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(Actor);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);

}
