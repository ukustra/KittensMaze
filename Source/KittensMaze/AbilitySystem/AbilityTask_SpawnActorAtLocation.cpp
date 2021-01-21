// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "AbilityTask_SpawnActorAtLocation.h"

UAbilityTask_SpawnActorAtLocation::UAbilityTask_SpawnActorAtLocation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UAbilityTask_SpawnActorAtLocation* UAbilityTask_SpawnActorAtLocation::SpawnActorAtLocation(UGameplayAbility* OwningAbility, TSubclassOf<AActor> Class, const FTransform& Transform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, AActor* Owner)
{
	UAbilityTask_SpawnActorAtLocation* MyObj = NewAbilityTask<UAbilityTask_SpawnActorAtLocation>(OwningAbility);

	MyObj->CachedTransform = Transform;
	MyObj->CachedCollisionHandling = CollisionHandlingOverride;
	MyObj->CachedActorOwner = Owner;

	return MyObj;
}

bool UAbilityTask_SpawnActorAtLocation::BeginSpawningActor(UGameplayAbility* OwningAbility, FGameplayAbilityTargetDataHandle TargetData, TSubclassOf<AActor> Class, AActor*& SpawnedActor)
{
	if (Ability && Ability->GetCurrentActorInfo()->IsNetAuthority() && ShouldBroadcastAbilityTaskDelegates())
	{
		UWorld* const World = GEngine->GetWorldFromContextObject(OwningAbility, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			SpawnedActor = World->SpawnActorDeferred<AActor>(Class, FTransform::Identity, NULL, NULL, CachedCollisionHandling);
		}
	}

	if (SpawnedActor == nullptr)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(nullptr);
		}
		return false;
	}

	return true;
}

void UAbilityTask_SpawnActorAtLocation::FinishSpawningActor(UGameplayAbility* OwningAbility, FGameplayAbilityTargetDataHandle TargetData, AActor* SpawnedActor)
{
	if (SpawnedActor)
	{
		SpawnedActor->SetOwner(CachedActorOwner);
		SpawnedActor->FinishSpawning(CachedTransform);

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Success.Broadcast(SpawnedActor);
		}
	}

	EndTask();
}
