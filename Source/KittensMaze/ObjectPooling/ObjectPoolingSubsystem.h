// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "KittensMaze/KittensMazeTypes.h"
#include "ObjectPoolingSubsystem.generated.h"

USTRUCT()
struct FPool
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<AObjectPoolingActor*> PooledActors;
};

/**
 * A World Subsystem for Actors with Object Pooling.
 */
UCLASS()
class KITTENSMAZE_API UObjectPoolingSubsystem final : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ObjectPooling", meta = (AdvancedDisplay = 2))
	bool ActivateActorFromPool(UPARAM(meta = (Categories = "ObjectPooling")) const FGameplayTag PoolingTag, const FTransform& WorldTransform, UObject* AdditionalData = nullptr);
	
	//~ Begin UWorldSubsystem interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~ End UWorldSubsystem interface

private:
	UPROPERTY()
	TMap<FGameplayTag, FObjectPoolingInfo> PooledActorsData;

	UPROPERTY()
	TMap<FGameplayTag, FPool> Pool;
};
