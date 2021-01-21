// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "KittensMazePickupActor.generated.h"

class USphereComponent;
class UAudioComponent;
class URotatingMovementComponent;
class AVoxelWorld;

UCLASS()
class KITTENSMAZE_API AKittensMazePickupActor : public AActor
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	URotatingMovementComponent* RotatingMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voxel", meta = (Categories = "WorldActor"))
	FGameplayTag VoxelWorldTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voxel")
	float PickupUnderVoxelThreshold;

	//~ Begin AActor interface
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	//~ End AActor interface

private:
	UPROPERTY()
	AVoxelWorld* VoxelWorld;

	float VoxelValue;
};
