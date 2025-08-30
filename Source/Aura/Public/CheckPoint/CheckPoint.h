// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Interaction/HighlightInterface.h"
#include "../Aura.h"
#include "CheckPoint.generated.h"

class USphereComponent;

/**
 * 
 */
UCLASS()
class AURA_API ACheckPoint : public APlayerStart, public ISaveInterface,public IHighlightInterface
{
	GENERATED_BODY()

public:

	ACheckPoint(const FObjectInitializer& ObjectInitializer);


	/*Save Interface*/
	bool ShouldLoadTransform_Implementation() { return false; };
	void LoadActor_Implementation();
	/*end Save Interface*/

	UPROPERTY(BlueprintReadWrite,SaveGame)
	bool bReached = false;

	UPROPERTY(EditAnywhere)
	bool  bBindOverlapCallback = true;


	UPROPERTY(EditDefaultsOnly)
	int32 CustomDepthStencilOverride = CUSTOM_DEPTH_TAN;


protected:

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay()override;

	/*Hightlight Interface*/
	void SetMoveToLocation_Implementation(FVector& OutDestination);
	void HighlightActor_Implementation();
	void UnHighlightActor_Implementation();
	/*end Hightlight Interface*/

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent>MoveToComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintCallable)
	void HandleGlowEffects();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent>CheckPointMesh;
		
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent>Sphere;

};
