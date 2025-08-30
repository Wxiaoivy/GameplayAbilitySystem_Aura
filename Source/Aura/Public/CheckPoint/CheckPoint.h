// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "CheckPoint.generated.h"

class USphereComponent;

/**
 * 
 */
UCLASS()
class AURA_API ACheckPoint : public APlayerStart, public ISaveInterface
{
	GENERATED_BODY()

public:

	ACheckPoint(const FObjectInitializer& ObjectInitializer);


	/*Save Interface*/
	bool ShouldLoadTransform_Implementation() { return false; };
	void LoadActor_Implementation();
	/*end Save Interface*/

	UPROPERTY(BlueprintReadOnly,SaveGame)
	bool bReached = false;

protected:

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay()override;

	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

	void HandleGlowEffects();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent>CheckPointMesh;

private:
		
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent>Sphere;

	
};
