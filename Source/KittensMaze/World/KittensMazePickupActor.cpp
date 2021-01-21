// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazePickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "KittensMaze/KittensMazeWorldSettings.h"
#include "VoxelWorld.h"
#include "VoxelTools/VoxelDataTools.h"

AKittensMazePickupActor::AKittensMazePickupActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PickupUnderVoxelThreshold(-0.085f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComponent->SetCanEverAffectNavigation(false);
	SetRootComponent(MeshComponent);

	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapOnlyPlayer"));
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->SetupAttachment(MeshComponent);

	AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	AudioComponent->bOverrideAttenuation = true;
	AudioComponent->AttenuationOverrides.DistanceAlgorithm = EAttenuationDistanceModel::Logarithmic;
	AudioComponent->AttenuationOverrides.AttenuationShapeExtents = FVector(300.f);
	AudioComponent->AttenuationOverrides.FalloffDistance = 200.f;
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(MeshComponent);

	RotatingMovementComponent = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, TEXT("RotatingMovementComponent"));
	RotatingMovementComponent->RotationRate = FRotator(0.f, 360.f, 0.f);
}

void AKittensMazePickupActor::BeginPlay()
{
	if (VoxelWorldTag.IsValid() && GetWorld())
	{
		if (const AKittensMazeWorldSettings* WorldSettings = Cast<AKittensMazeWorldSettings>(GetWorld()->GetWorldSettings()))
		{
			VoxelWorld = Cast<AVoxelWorld>(WorldSettings->GetWorldActor(VoxelWorldTag));
			SetActorTickEnabled(true);
		}
	}
	
	Super::BeginPlay();
}

void AKittensMazePickupActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AudioComponent && VoxelWorld && VoxelWorld->IsCreated())
	{
		float NewValue;
		UVoxelDataTools::GetValue(NewValue, VoxelWorld, VoxelWorld->GlobalToLocal(GetActorLocation()));

		if (VoxelValue != NewValue)
		{
			VoxelValue = NewValue;
			
			if (VoxelValue < PickupUnderVoxelThreshold)
			{
				if (!AudioComponent->IsPlaying())
				{
					AudioComponent->Play();
				}
			}
			else if (AudioComponent->IsPlaying())
			{
				AudioComponent->Stop();
			}
		}
	}
}

void AKittensMazePickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	Destroy();
}
