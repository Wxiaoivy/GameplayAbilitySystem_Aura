// Copyright Ivy


#include "Actor/PointCollection.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
APointCollection::APointCollection()
{

	PrimaryActorTick.bCanEverTick = false;

	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePts.Add(Pt_0);
	SetRootComponent(Pt_0);

	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");
	ImmutablePts.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());

	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	ImmutablePts.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());

	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	ImmutablePts.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());

	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	ImmutablePts.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());

	Pt_5 = CreateDefaultSubobject<USceneComponent>("Pt_5");
	ImmutablePts.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());

	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	ImmutablePts.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());

	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	ImmutablePts.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());

	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	ImmutablePts.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());

	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	ImmutablePts.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());

	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	ImmutablePts.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());

}


TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride /*= 0.f*/)
{
	// 安全检查：确保请求的点数不超过ImmutablePts的总数
	checkf(ImmutablePts.Num() >= NumPoints, TEXT("Attempt to access ImmutablePts out of bounds."));
	// 存储处理后的点副本
	TArray<USceneComponent*>ArrayCopy;
	// 遍历所有固定点
	for (USceneComponent* Pt : ImmutablePts)
	{
		// 如果已收集足够点数，提前返回结果
		if (ArrayCopy.Num() >= NumPoints)return ArrayCopy;

		// 对非中心参考点（Pt_0）进行位置调整（应用偏航旋转）
		if (Pt != Pt_0)
		{
			// 计算当前点相对于参考点（Pt_0）的偏移向量
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			// 绕Z轴（UpVector）旋转偏移向量（YawOverride度）
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			// 更新当前点位置（保持旋转后的相对偏移）
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}

		// 地面检测准备：
		// 在当前位置正上方500单位（RaisedLocation）和正下方500单位（LoweredLocation）创建检测点
		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 500.f);

		
		
		// 忽略周围1500单位内的存活玩家（防止射线打到玩家）
		TArray<AActor*>IgnoreActors;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

		// 执行射线检测（检测地面碰撞）:
		// 设置射线检测参数
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		// 从上往下发射射线，检测"BlockAll"碰撞类型的表面（如地面）
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"),QueryParams);
		// 调整当前点高度至射线命中点（ImpactPoint）的Z坐标
		const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
		Pt->SetWorldLocation(AdjustedLocation);
		// 根据地面法线（ImpactNormal）旋转点，使Z轴垂直于地面
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));//不用管具体旋转多少度，UE 的 MakeRotFromZ 会自动帮你计算正确的旋转，让贴花 / 植被的“正面”牢牢贴住地面（对齐表面法线）。
		                                                                               //用 MakeRotFromZ(Normal) 可以让植被的“根部”垂直扎进地面，但如果你想要随机旋转（比如让草 / 石头方向不全是同一个朝向），可以额外加一个 Yaw（水平旋转）随机化：

		 // 将处理后的点加入返回数组
		ArrayCopy.Add(Pt);
	}
	return ArrayCopy;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}


