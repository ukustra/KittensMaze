// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "ObjectPoolingSubsystem.h"

#include "Kismet/KismetSystemLibrary.h"
#include "KittensMaze/KittensMazeWorldSettings.h"

bool UObjectPoolingSubsystem::ActivateActorFromPool(const FGameplayTag PoolingTag, const FTransform& WorldTransform, UObject* AdditionalData)
{
    if (Pool.Contains(PoolingTag) && PooledActorsData.Contains(PoolingTag))
    {
        const FObjectPoolingInfo& PoolingInfo = PooledActorsData[PoolingTag];
        
        if (PoolingInfo.IsValid())
        {
            AObjectPoolingActor* FoundActor = Invoke([this, PoolingTag]()->AObjectPoolingActor*
            {
                const TArray<AObjectPoolingActor*>& PooledActors= Pool[PoolingTag].PooledActors;
                for (AObjectPoolingActor* Actor : PooledActors)
                {
                    if (Actor && !Actor->IsActive())
                    {
                        return Actor;
                    }
                }

                const int32 LastIndex = PooledActors.Num() - 1;
                for (int32 i = 0; i < LastIndex; ++i)
                {
                    if (AObjectPoolingActor* Actor = PooledActors[i])
                    {
                        const int32 NextIndex = i + 1;
                        AObjectPoolingActor* NextActor = PooledActors[NextIndex];
                        const bool bNextActorCheck = !NextActor || (NextActor && NextActor->IsActive());
                        if (bNextActorCheck)
                        {
                            Actor->SetActive(false);
                            return Actor;
                        }
                    }
                }

                if (AObjectPoolingActor* LastActor = PooledActors[LastIndex])
                {
                    LastActor->SetActive(false);
                    return LastActor;
                }

                return nullptr;
            });

            if (FoundActor)
            {
                FoundActor->SetActorTransform(WorldTransform);
                FoundActor->SetAdditionalData(AdditionalData);
                FoundActor->SetLifeSpan(PoolingInfo.LifeSpan);

                return true;
            }
        }
    }

    return false;
}

bool UObjectPoolingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (GEngine)
    {
        if (const UWorld* World = GEngine->GetWorldFromContextObject(Outer, EGetWorldErrorMode::LogAndReturnNull))
        {
            if (const AKittensMazeWorldSettings* WorldSettings = Cast<AKittensMazeWorldSettings>(World->GetWorldSettings()))
            {
                return WorldSettings->GetInitWorldSubsystems();
            }
        }
    }

    return false;
}

void UObjectPoolingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    if (!GEngine)
    {
        return;
    }

    if (UWorld* World = GEngine->GetWorldFromContextObject(GetOuter(), EGetWorldErrorMode::LogAndReturnNull))
    {
        if (const AKittensMazeWorldSettings* WorldSettings = Cast<AKittensMazeWorldSettings>(World->GetWorldSettings()))
        {
            PooledActorsData = WorldSettings->GetPooledActorsData();

            for (const auto& It : PooledActorsData)
            {
                const FGameplayTag& PoolingTag = It.Key;
                const FObjectPoolingInfo& PoolingInfo = It.Value;
                if (PoolingInfo.IsValid())
                {
                    FPool CurrentPool;
                    for (int32 i = 0; i < PoolingInfo.PoolSize; ++i)
                    {
                        AObjectPoolingActor* PooledActor = World->SpawnActorDeferred<AObjectPoolingActor>(PoolingInfo.ActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
                        PooledActor->SetActive(false);
                        CurrentPool.PooledActors.Add(PooledActor);
                        PooledActor->FinishSpawning(FTransform::Identity);
                    }

                    Pool.Add(PoolingTag, CurrentPool);
                }
            }
        }
    }
}
