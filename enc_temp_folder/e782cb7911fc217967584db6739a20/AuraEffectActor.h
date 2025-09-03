// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Player/AuraPlayerState.h"
#include "AuraEffectActor.generated.h"


class IAbilitySystemInterface;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy:uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply

};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy:uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AAuraEffectActor();

	virtual void Tick(float DeltaSeconds)override;


protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	FVector CalculatedLocation;

	UPROPERTY(BlueprintReadOnly)
	FRotator CalcultedRotation;


	/*参数调整建议
		想要更大浮动幅度：增加 SineAmplitude

		想要更快振荡：增加 SinePeriodConstant

		想要更慢振荡：减小 SinePeriodConstant

		想要水平运动：改为 FVector(Sine, 0.f, 0.f) 或 FVector(0.f, Sine, 0.f)

		想要圆形运动：同时在X和Y轴使用正弦和余弦

		这就是创建平滑悬浮效果的经典方法！*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Pickup Movement")
	bool bRotates = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float RotationRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	bool bSinusoidalMovement = false;

	UFUNCTION(BlueprintCallable)
	void StartRotation();

	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float SineAmplitude = 1.f;

	//周期常数（SinePeriodConstant）是一个控制正弦波振荡速度的缩放因子。
	/*可以把 SinePeriodConstant 理解为"频率调节器":
		值越大 → 正弦波振荡越快
		值越小 → 正弦波振荡越慢*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")	
	float SinePeriodConstant = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	FVector InitialLocation;






	// 功能：向目标应用一个游戏效果
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect>GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bApplyEffectOnEnemies = false;


	// 功能：存储一个游戏效果的类，这个效果可以瞬间应用
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect>InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect>DurationGameplayEffectClass;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect>InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;


	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*>ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects")
	float ActorLevel = 1.f;

private:

	float RunningTime = 0.f; //RunningTime：游戏运行时间（秒），通常来自 Tick 函数的 DeltaTime 累积
	void ItemMovement(float DeltaTime);
};
