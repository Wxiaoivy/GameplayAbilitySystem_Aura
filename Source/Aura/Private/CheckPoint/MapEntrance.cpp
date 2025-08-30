// Copyright Ivy


#include "CheckPoint/MapEntrance.h"
#include "Interaction/PlayerInterface.h"
#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

void AMapEntrance::HighlightActor_Implementation()
{
	CheckPointMesh->SetRenderCustomDepth(true);
}

void AMapEntrance::LoadActor_Implementation()
{
	//DO nothing when loading a Map Entrance
}

void AMapEntrance::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGM->SaveWorldState(GetWorld(),DestinationMap.ToSoftObjectPath().GetAssetName());
		}
		// ������ҽӿڵı�����ȷ��������������ʼ��ǩ
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);//PlayerStartTag�Ǹ����Դ��ġ�  ��ͼ�����ֱ�ӳ�ʼ����SaveProgress��ʵ��д��AuraCharacter����ģ�

		UGameplayStatics::OpenLevelBySoftObjectPtr(this,DestinationMap);
	}
}
