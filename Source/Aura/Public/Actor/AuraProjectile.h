// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Aura/Aura.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraProjectile.generated.h"


class UNiagaraSystem;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()

public:

	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent>ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))//ExposeOnSpawn:在 生成 Actor 或 Object 时，允许外部传入初始值（如初始位置、速度、生命值等）。
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:

	virtual void BeginPlay() override;
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent*OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void Destroyed()override;


private:

	bool bHit = false;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent>Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem>ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase>ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase>LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent>LoopingSoundComponent;


	

};