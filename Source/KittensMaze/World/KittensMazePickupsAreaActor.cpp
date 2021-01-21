// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazePickupsAreaActor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

AKittensMazePickupsAreaActor::AKittensMazePickupsAreaActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BoxComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxComponent"));
	BoxComponent->SetCanEverAffectNavigation(false);
	BoxComponent->SetCollisionProfileName(TEXT("NoCollision"));
	BoxComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(BoxComponent);

	ArrowComponent = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(BoxComponent);
}

void AKittensMazePickupsAreaActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearActors();

	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void AKittensMazePickupsAreaActor::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	InitActors();
}

void AKittensMazePickupsAreaActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InitActors();
}
#endif

void AKittensMazePickupsAreaActor::OnConstruction(const FTransform& Transform)
{
	InitActors();

	Super::OnConstruction(Transform);
}

bool AKittensMazePickupsAreaActor::CanSpawnPickupAtLocation(const FVector& Location) const
{
	if (GeneratedActors.Num() == 0)
	{
		return true;
	}
	else if (!GeneratedActors.Contains(Location))
	{
		for (const auto& It : GeneratedActors)
		{
			if (FVector::Dist(It.Key, Location) > MinDistanceBetweenPickups)
			{
				return true;
			}
		}
	}

	return false;
}

void AKittensMazePickupsAreaActor::InitActors()
{
	if (!GetWorld())
	{
		return;
	}

	ClearActors();

	MinDistanceBetweenPickups = FMath::Min(MinDistanceBetweenPickups, BoxComponent->Bounds.SphereRadius);

	const FActorSpawnParameters SpawnParams = Invoke([]()->FActorSpawnParameters const {
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		return Params;
	});

	for (const auto& PickupData : Pickups)
	{
		if (PickupData.PickupClass)
		{
			int32 Amount = FMath::RandRange(PickupData.MinAmount, PickupData.MaxAmount);
			for (int32 i = 0; i < Amount; ++i)
			{
				const FVector Origin = BoxComponent->Bounds.Origin;
				const FVector BoxExtent = BoxComponent->GetScaledBoxExtent();
				const FVector BoxMin = Origin - BoxExtent;
				const FVector BoxMax = Origin + BoxExtent;
				const FVector LocationCandidate = FMath::RandPointInBox(FBox(BoxMin, BoxMax));

				if (CanSpawnPickupAtLocation(LocationCandidate))
				{
					AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(PickupData.PickupClass, FTransform(LocationCandidate), SpawnParams);
					GeneratedActors.Add(LocationCandidate, SpawnedActor);
				}
			}
		}
	}
}

void AKittensMazePickupsAreaActor::ClearActors()
{
	for (auto& It : GeneratedActors)
	{
		if (It.Value)
		{
			It.Value->Destroy();
		}
	}
	GeneratedActors.Empty();
}
