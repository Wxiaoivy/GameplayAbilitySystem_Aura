// Copyright Ivy


#include "AbilitySystem/Abilities/AuraSummonAbility.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();// ��ȡ����ʩ���ߵ���ǰ������
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();// ��ȡ����ʩ���ߵ�λ��
	const float DeltaSpread = SpawnSpread / NumMinions;// ����ÿ��С��֮��ĽǶȼ��

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector); // �������������ķ�������(����������ת��һ��ķֲ��Ƕ�)
	                                                                                             // RotateAngleAxis():����ǰ�������ո����� ��ת�ᣨAxis�� �� �Ƕȣ�Angle�� ������ת����������ת�����������
	TArray<FVector>SpawnLocations;// �����洢����λ�õ�����

	for (int32 i = 0; i < NumMinions; i++)// ����ÿ��Ҫ���ɵ�С��
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);// ���㵱ǰС�������ɷ���(������࿪ʼ���������ӽǶ�)
		 FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);// �ڸ÷�����С��������֮�����ѡ��һ����������λ��

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);

			if (Hit.bBlockingHit)
			{
				ChosenSpawnLocation = Hit.ImpactPoint;
			}

		    SpawnLocations.Add(ChosenSpawnLocation); // ��λ����ӵ�������


		// ���Ի��ƣ�
	//	DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 18.f, 12.f, FColor::Green, false, 3.f);// ������λ�û�����ɫ����(�뾶18��ϸ��12������3��)	
	//	UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Blue, 3.f);// ��ʩ����λ�õ������������ɫ��ͷ(��ϸ4������3��)

	//	DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12.f, FColor::Red, false, 3.f);// ����С����λ�û��ƺ�ɫС��
	//	DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12.f, FColor::Red, false, 3.f);// ��������λ�û��ƺ�ɫС��
	//
	}
	return SpawnLocations; // ������������λ�õ�����
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num()-1);
	return MinionClasses[Selection];
}
