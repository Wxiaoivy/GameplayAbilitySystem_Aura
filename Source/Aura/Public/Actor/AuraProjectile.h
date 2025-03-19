// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AuraProjectile.generated.h"



UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()

public:

	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent>ProjectileMovement;

protected:

	virtual void BeginPlay() override;
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent*OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent>Sphere;

	

};