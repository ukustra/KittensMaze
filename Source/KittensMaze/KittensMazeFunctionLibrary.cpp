// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazeFunctionLibrary.h"
#include "KittensMazeSettings.h"
#include "KittensMazeWorldSettings.h"
#include "KittensMazeGameInstance.h"
#include "LatentActions.h"

class FSpawnActorAtRandomTargetPointsAction : public FPendingLatentAction
{

public:
	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	TArray<AActor*> TargetActors;

	int32 RandomActorsAmount;
	TSubclassOf<AActor> ActorClassToSpawn;
	float MinYawOffset;
	float MaxYawOffset;
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandling;
	bool bDestroyTargetPointsAftersSpawn;

	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

	FSpawnActorAtRandomTargetPointsAction(UWorld* InWorld, const TArray<AActor*>& InTargetActors, int32 InRandomActorsAmount, const TSubclassOf<AActor>& InActorClassToSpawn,
		float InMinYawOffset, float InMaxYawOffset, ESpawnActorCollisionHandlingMethod InSpawnCollisionHandling, bool bInDestroyTargetPointsAftersSpawn, const FLatentActionInfo& LatentInfo)
		: World(InWorld)
		, TargetActors(InTargetActors)
		, RandomActorsAmount(InRandomActorsAmount)
		, ActorClassToSpawn(InActorClassToSpawn)
		, MinYawOffset(InMinYawOffset)
		, MaxYawOffset(InMaxYawOffset)
		, SpawnCollisionHandling(InSpawnCollisionHandling)
		, bDestroyTargetPointsAftersSpawn(bInDestroyTargetPointsAftersSpawn)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
	{

	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		if (World)
		{
			if (RandomActorsAmount >= TargetActors.Num())
			{
				RandomTargetActors = TargetActors;
				SpawnActors();
			}
			else
			{
				while (RandomIndexes.Num() != RandomActorsAmount)
				{
					const int32 RandomIndex = FMath::RandHelper(TargetActors.Num());
					RandomIndexes.Add(RandomIndex);
				}

				for (const auto& Index : RandomIndexes)
				{
					RandomTargetActors.Add(TargetActors[Index]);
				}

				SpawnActors();
			}
			
			Response.FinishAndTriggerIf(bAllSpawned, ExecutionFunction, OutputLink, CallbackTarget);
		}
		else
		{
			Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
		}
	}

private:
	UPROPERTY()
	TArray<AActor*> RandomTargetActors;

	TSet<int32> RandomIndexes;
	bool bAllSpawned;

	void SpawnActors()
	{
		for (AActor* RandomActor : RandomTargetActors)
		{
			if (RandomActor)
			{
				const FRotator RotationOffset = FRotator(0.f, FMath::RandRange(MinYawOffset, MaxYawOffset), 0.f);
				const FQuat ActorQuat = RandomActor->GetActorQuat() * RotationOffset.Quaternion();
				const FTransform SpawnTransform = FTransform(ActorQuat, RandomActor->GetActorLocation());

				AActor* SpawnedActor = World->SpawnActorDeferred<AActor>(ActorClassToSpawn, SpawnTransform, nullptr, nullptr, SpawnCollisionHandling);
				SpawnedActor->FinishSpawning(SpawnTransform);
			}
		}

		if (bDestroyTargetPointsAftersSpawn)
		{
			for (AActor* Actor : TargetActors)
			{
				if (Actor)
				{
					Actor->Destroy();
				}
			}
		}

		bAllSpawned = true;
	}
};

float UKittensMazeFunctionLibrary::GetMinDamageToNormalize()
{
	if (const UKittensMazeSettings* Settings = UKittensMazeSettings::Get())
	{
		return Settings->GetMinDamageToNormalize();
	}

	return 0.f;
}

float UKittensMazeFunctionLibrary::GetMaxDamageToNormalize()
{
	if (const UKittensMazeSettings* Settings = UKittensMazeSettings::Get())
	{
		return Settings->GetMaxDamageToNormalize();
	}

	return 0.f;
}

float UKittensMazeFunctionLibrary::GetBombDamageMultiplier()
{
	if (const UKittensMazeSettings* Settings = UKittensMazeSettings::Get())
	{
		return Settings->GetBombDamageMultiplier();
	}

	return 0.f;
}

AActor* UKittensMazeFunctionLibrary::GetWorldActor(const UObject* WorldContextObject, const FGameplayTag ActorTag)
{
	if (GEngine)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (const AKittensMazeWorldSettings* KittensWorldSettings = Cast<AKittensMazeWorldSettings>(World->GetWorldSettings()))
			{
				return KittensWorldSettings->GetWorldActor(ActorTag);
			}
		}
	}

	return nullptr;
}

UKittensMazeGameInstance* UKittensMazeFunctionLibrary::GetKittensMazeGameInstance(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return Cast<UKittensMazeGameInstance>(World->GetGameInstance());
	}

	return nullptr;
}

USaveGame* UKittensMazeFunctionLibrary::GetSaveGameFromSlot(const UObject* WorldContextObject, const FString Slot)
{
	if (const UKittensMazeGameInstance* GameInstance = UKittensMazeFunctionLibrary::GetKittensMazeGameInstance(WorldContextObject))
	{
		return GameInstance->GetSaveGameFromSlot(Slot);
	}

	return nullptr;
}

ULocalPlayer* UKittensMazeFunctionLibrary::GetLocalPlayer(const APlayerController* PlayerController)
{
	return PlayerController ? PlayerController->GetLocalPlayer() : nullptr;
}

void UKittensMazeFunctionLibrary::SpawnActorAtRandomTargetPoints(const UObject* WorldContextObject, const TArray<AActor*>& TargetPoints, int32 RandomActorsAmount, TSubclassOf<AActor> ActorClassToSpawn,
	struct FLatentActionInfo LatentInfo, float MinYawOffset, float MaxYawOffset, ESpawnActorCollisionHandlingMethod SpawnCollisionHandling, bool bDestroyTargetPointsAftersSpawn)
{
	if (ActorClassToSpawn && TargetPoints.Num() > 0 && RandomActorsAmount > 0)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

			if (!LatentActionManager.FindExistingAction<FSpawnActorAtRandomTargetPointsAction>(LatentInfo.CallbackTarget, LatentInfo.UUID))
			{
				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID,
					new FSpawnActorAtRandomTargetPointsAction(World, TargetPoints, RandomActorsAmount, ActorClassToSpawn, MinYawOffset, MaxYawOffset, SpawnCollisionHandling, bDestroyTargetPointsAftersSpawn, LatentInfo));
			}
		}
	}
}

FGameplayEffectContextHandle UKittensMazeFunctionLibrary::GetEffectContextFromSpec(const FGameplayEffectSpec& Spec)
{
	return Spec.GetEffectContext();
}
