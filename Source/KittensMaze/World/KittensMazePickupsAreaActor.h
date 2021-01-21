// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KittensMazePickupsAreaActor.generated.h"

class UBoxComponent;
class UArrowComponent;

USTRUCT(BlueprintType)
struct FPickupData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
	int32 MinAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
	int32 MaxAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> PickupClass;

	FORCEINLINE friend uint32 GetTypeHash(const FPickupData& Data)
	{
		return GetTypeHash(Data.PickupClass);
	}
};

UCLASS()
class KITTENSMAZE_API AKittensMazePickupsAreaActor : public AActor
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	TSet<FPickupData> Pickups;

	UPROPERTY(EditAnywhere, Category = "Pickup", meta = (ClampMin = 0.f))
	float MinDistanceBetweenPickups;

	//~ Begin AActor interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void OnConstruction(const FTransform& Transform) override;
	//~ End AActor interface

	bool CanSpawnPickupAtLocation(const FVector& Location) const;

private:
	UPROPERTY()
	TMap<FVector, AActor*> GeneratedActors;

	void InitActors();
	void ClearActors();
};
