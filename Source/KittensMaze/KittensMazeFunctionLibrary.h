// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "KittensMazeFunctionLibrary.generated.h"

class UKittensMazeGameInstance;
class USaveGame;

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UKittensMazeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Damage")
	static float GetMinDamageToNormalize();

	UFUNCTION(BlueprintPure, Category = "Damage")
	static float GetMaxDamageToNormalize();

	UFUNCTION(BlueprintPure, Category = "Damage")
	static float GetBombDamageMultiplier();

	UFUNCTION(BlueprintPure, Category = "Voxel", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext = true))
	static AActor* GetWorldActor(const UObject* WorldContextObject, UPARAM(meta = (Categories = "WorldActor")) const FGameplayTag ActorTag);

	UFUNCTION(BlueprintPure, Category = "Game", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext = true))
	static UKittensMazeGameInstance* GetKittensMazeGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Game", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext = true))
	static USaveGame* GetSaveGameFromSlot(const UObject* WorldContextObject, const FString Slot);

	UFUNCTION(BlueprintPure, Category = "Game")
	static ULocalPlayer* GetLocalPlayer(const APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Game", meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", AdvancedDisplay = 5))
	static void SpawnActorAtRandomTargetPoints(const UObject* WorldContextObject, const TArray<AActor*>& TargetPoints, int32 RandomActorsAmount,
		TSubclassOf<AActor> ActorClassToSpawn, struct FLatentActionInfo LatentInfo, float MinYawOffset = -90.f, float MaxYawOffset = 90.f,
		ESpawnActorCollisionHandlingMethod SpawnCollisionHandling = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, bool bDestroyTargetPointsAftersSpawn = true);

	UFUNCTION(BlueprintPure, Category = "Ability|GameplayEffect")
	static FGameplayEffectContextHandle GetEffectContextFromSpec(const FGameplayEffectSpec& Spec);
};
