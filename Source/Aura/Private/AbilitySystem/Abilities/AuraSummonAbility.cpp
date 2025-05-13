// Copyright Ivy


#include "AbilitySystem/Abilities/AuraSummonAbility.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();// 获取技能施放者的正前方向量
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();// 获取技能施放者的位置
	const float DeltaSpread = SpawnSpread / NumMinions;// 计算每个小兵之间的角度间隔

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector); // 计算扇形最左侧的方向向量(绕上向量旋转负一半的分布角度)
	                                                                                             // RotateAngleAxis():将当前向量按照给定的 旋转轴（Axis） 和 角度（Angle） 进行旋转，并返回旋转后的新向量。
	TArray<FVector>SpawnLocations;// 创建存储生成位置的数组

	for (int32 i = 0; i < NumMinions; i++)// 遍历每个要生成的小兵
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);// 计算当前小兵的生成方向(从最左侧开始，依次增加角度)
		 FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);// 在该方向最小和最大距离之间随机选择一个距离生成位置

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);

			if (Hit.bBlockingHit)
			{
				ChosenSpawnLocation = Hit.ImpactPoint;
			}

		    SpawnLocations.Add(ChosenSpawnLocation); // 将位置添加到数组中


		// 调试绘制：
	//	DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 18.f, 12.f, FColor::Green, false, 3.f);// 在生成位置绘制绿色球体(半径18，细分12，持续3秒)	
	//	UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Blue, 3.f);// 从施法者位置到最大距离绘制蓝色箭头(粗细4，持续3秒)

	//	DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12.f, FColor::Red, false, 3.f);// 在最小距离位置绘制红色小球
	//	DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12.f, FColor::Red, false, 3.f);// 在最大距离位置绘制红色小球
	//
	}
	return SpawnLocations; // 返回所有生成位置的数组
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num()-1);
	return MinionClasses[Selection];
}
