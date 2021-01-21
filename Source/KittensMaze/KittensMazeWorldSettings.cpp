// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazeWorldSettings.h"
#include "Components/MaterialBouncingComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "VoxelWorld.h"
#include "UI/KittensMazeFocusWidget.h"

AKittensMazeWorldSettings::AKittensMazeWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetRootComponent(RootComponent);

	MaterialBouncingComponent = ObjectInitializer.CreateDefaultSubobject<UMaterialBouncingComponent>(this, TEXT("MaterialBouncingComponent"));
}

bool AKittensMazeWorldSettings::GetInitWorldSubsystems() const
{
	return bInitWorldSubsystems;
}

const TMap<FGameplayTag, FObjectPoolingInfo>& AKittensMazeWorldSettings::GetPooledActorsData() const
{
	return PooledActorsData;
}

AActor* AKittensMazeWorldSettings::GetWorldActor(const FGameplayTag& ActorTag) const
{
	return WorldActors.Contains(ActorTag) ? WorldActors[ActorTag] : nullptr;
}

TSet<AVoxelWorld*> AKittensMazeWorldSettings::GetAllVoxelWorlds() const
{
	TSet<AVoxelWorld*> Voxels;
	for (const auto& It : WorldActors)
	{
		if (AVoxelWorld* VoxelWorld = Cast<AVoxelWorld>(It.Value))
		{
			Voxels.Add(VoxelWorld);
		}
	}

	return Voxels;
}

bool AKittensMazeWorldSettings::AreAllVoxelWorldsLoaded() const
{
	for (const auto& It : WorldActors)
	{
		AVoxelWorld* VoxelWorld = Cast<AVoxelWorld>(It.Value);
		if (VoxelWorld && !VoxelWorld->IsLoaded())
		{
			return false;
		}
	}

	return true;
}

void AKittensMazeWorldSettings::InitMainMenuWigdet(APlayerController* OwningController)
{
	if (MainMenuWidgetClass && !MainMenuWidget.IsValid())
	{
		MainMenuWidget = CreateWidget<UKittensMazeFocusWidget>(OwningController, MainMenuWidgetClass);
		MainMenuWidget.Get()->AddToViewport();
	}
}

bool AKittensMazeWorldSettings::ToggleMainMenuWidget(APlayerController* OwningPlayerController)
{
	if (!GetPauserPlayerState() && OwningPlayerController && MainMenuWidgetClass)
	{
		if (MainMenuWidget.IsValid())
		{
			if (MainMenuWidget.Get()->CanBeRemoved())
			{
				ResetMainMenuWidgetInternal();
			}
		}
		else
		{
			MainMenuWidget = CreateWidget<UKittensMazeFocusWidget>(OwningPlayerController, MainMenuWidgetClass);
			MainMenuWidget.Get()->AddToViewport();
		}

		return true;
	}

	return false;
}

bool AKittensMazeWorldSettings::ResetMainMenuWidget(UKittensMazeFocusWidget* InMainMenuWidget)
{
	if (MainMenuWidget.IsValid() && MainMenuWidget.Get()->CanBeRemoved() && MainMenuWidget.Get() == InMainMenuWidget)
	{
		ResetMainMenuWidgetInternal();

		return true;
	}

	return false;
}

void AKittensMazeWorldSettings::SetPauserPlayerState(APlayerState* PlayerState)
{
	Super::SetPauserPlayerState(PlayerState);

	const bool bGamePaused = GetPauserPlayerState() != nullptr;
	OnGamePaused.Broadcast(bGamePaused);
}

void AKittensMazeWorldSettings::PostInitializeComponents()
{
	InitVoxels();

	Super::PostInitializeComponents();
}

void AKittensMazeWorldSettings::OnPointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	for (const auto& It : WorldActors)
	{
		AVoxelWorld* VoxelWorld = Cast<AVoxelWorld>(It.Value);
		if (VoxelWorld && VoxelWorld == DamagedActor)
		{
			MaterialBouncingComponent->Bounce(Damage, HitLocation, ShotFromDirection);

			if (VoxelEventTag.IsValid() && InstigatedBy && InstigatedBy->GetPawn())
			{
				if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstigatedBy->GetPawn()))
				{
					const FHitResult HitResult = Invoke([HitLocation, ShotFromDirection]()->FHitResult const {
						FHitResult HR;
						HR.Location = HitLocation;
						HR.Normal = ShotFromDirection;

						return HR;
					});

					const FGameplayEffectContextHandle ContextHandle = Invoke([AbilitySystemComponent, HitResult]()->FGameplayEffectContextHandle const {
						FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
						Context.AddHitResult(HitResult);

						return Context;
					});

					const FGameplayEventData Payload = Invoke([this, InstigatedBy, VoxelWorld, Damage, &ContextHandle]()->FGameplayEventData const {
						FGameplayEventData EventData;
						EventData.Instigator = InstigatedBy->GetPawn();
						EventData.Target = VoxelWorld;
						EventData.ContextHandle = ContextHandle;
						EventData.EventMagnitude = Damage;

						return EventData;
					});

					FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
					AbilitySystemComponent->HandleGameplayEvent(VoxelEventTag, &Payload);
				}
			}
		}
	}
}

void AKittensMazeWorldSettings::InitVoxels()
{
	for (const auto& It : WorldActors)
	{
		if (AVoxelWorld* VoxelWorld = Cast<AVoxelWorld>(It.Value))
		{
			if (UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(VoxelWorld->VoxelMaterial, this, NAME_None))
			{
				VoxelWorld->VoxelMaterial = DynamicMat;
				MaterialBouncingComponent->InitDynamicMaterial(DynamicMat);
			}

			VoxelWorld->OnTakePointDamage.AddDynamic(this, &AKittensMazeWorldSettings::OnPointDamage);
		}
	}
}

void AKittensMazeWorldSettings::ResetMainMenuWidgetInternal()
{
	MainMenuWidget.Get()->RemoveFromParent();
	MainMenuWidget = nullptr;
}
