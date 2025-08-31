// Copyright Ivy


#include "Actor/AuraEnemySpawnVolume.h"
#include "Interaction/PlayerInterface.h"
#include "Actor/AuraEnemySpawnPoint.h"

AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)
	{
		Destroy();
	}
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnBoxBeginOverlap);
}

void AAuraEnemySpawnVolume::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
	}
	for (AAuraEnemySpawnPoint* Point:SpawnPoints)
	{
		if (IsValid(Point))
		{
			Point->SpawnEnemy();
		}
	}
	//Destroy();这里不能销毁，如果销毁了在保存游戏的时候这个Actor就无效不会被保存， 下次LoadActor的时候会自动摧毁
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


