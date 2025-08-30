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
		// 调用玩家接口的保存进度方法，传递玩家起始标签
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);//PlayerStartTag是该类自带的。  蓝图里可以直接初始化（SaveProgress的实现写在AuraCharacter里面的）

		UGameplayStatics::OpenLevelBySoftObjectPtr(this,DestinationMap);
	}
}
