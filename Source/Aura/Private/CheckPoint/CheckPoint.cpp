// Copyright Ivy


#include "CheckPoint/CheckPoint.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h>
#include "Interaction/PlayerInterface.h"
#include "Game/AuraGameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

ACheckPoint::ACheckPoint(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckPointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckPointMesh");
	CheckPointMesh->SetupAttachment(GetRootComponent());
	CheckPointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckPointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckPointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CheckPointMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride);
	CheckPointMesh->MarkRenderStateDirty();

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(Sphere);
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();

	if (bBindOverlapCallback)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnSphereBeginOverlap);
	}
}

//作用：检查点Actor的加载实现，当从存档加载时，如果检查点之前已被激活，则恢复其视觉效果。
void ACheckPoint::LoadActor_Implementation()
{
	// 如果检查点之前已经被到达过（bReached为true）
	if (bReached)
	{
		// 处理发光效果（可能是激活状态的效果）
		HandleGlowEffects();
	}
}

//作用：当玩家进入检查点触发范围时，标记检查点为已到达状态，保存游戏进度，并触发视觉效果。
void ACheckPoint::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 检查重叠的Actor是否实现了PlayerInterface（应该是玩家角色）
	if (OtherActor->Implements<UPlayerInterface>())
	{
			// 标记检查点已被到达
			bReached = true;
			// 获取游戏模式并保存世界状态
			if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
			{
				const UWorld* World = GetWorld();
				FString MapName = World->GetMapName();
				MapName.RemoveFromStart(World->StreamingLevelsPrefix);
				AuraGM->SaveWorldState(GetWorld());
			}
		
		// 调用玩家接口的保存进度方法，传递玩家起始标签
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);//PlayerStartTag是该类自带的。  蓝图里可以直接初始化（SaveProgress的实现写在AuraCharacter里面的）
		// 处理发光效果（激活检查点时的视觉效果）
		HandleGlowEffects();
	}
}



void ACheckPoint::HighlightActor_Implementation()
{
	if (!bReached)
	{
		CheckPointMesh->SetRenderCustomDepth(true);
	}
}

void ACheckPoint::UnHighlightActor_Implementation()
{
	CheckPointMesh->SetRenderCustomDepth(false);
}

void ACheckPoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}


void ACheckPoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckPointMesh->GetMaterial(0), this);
	CheckPointMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckPointReached(DynamicMaterialInstance);
}
