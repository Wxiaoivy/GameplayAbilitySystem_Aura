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

	CheckPointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	CheckPointMesh->MarkRenderStateDirty();

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(Sphere);
}

//���ã�����Actor�ļ���ʵ�֣����Ӵ浵����ʱ���������֮ǰ�ѱ������ָ����Ӿ�Ч����
void ACheckPoint::LoadActor_Implementation()
{
	// �������֮ǰ�Ѿ����������bReachedΪtrue��
	if (bReached)
	{
		// ������Ч���������Ǽ���״̬��Ч����
		HandleGlowEffects();
	}
}

//���ã�����ҽ�����㴥����Χʱ����Ǽ���Ϊ�ѵ���״̬��������Ϸ���ȣ��������Ӿ�Ч����
void ACheckPoint::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ����ص���Actor�Ƿ�ʵ����PlayerInterface��Ӧ������ҽ�ɫ��
	if (OtherActor->Implements<UPlayerInterface>())
	{
			// ��Ǽ����ѱ�����
			bReached = true;
			// ��ȡ��Ϸģʽ����������״̬
			if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
			{
				AuraGM->SaveWorldState(GetWorld());
			}
		
		// ������ҽӿڵı�����ȷ��������������ʼ��ǩ
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);//PlayerStartTag�Ǹ����Դ��ġ�  ��ͼ�����ֱ�ӳ�ʼ����SaveProgress��ʵ��д��AuraCharacter����ģ�
		// ������Ч�����������ʱ���Ӿ�Ч����
		HandleGlowEffects();
	}
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnSphereBeginOverlap);
}

void ACheckPoint::HighlightActor_Implementation()
{
	CheckPointMesh->SetRenderCustomDepth(true);
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
