// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "GameplayTagContainer.h"
#include "KittensMazeTypes.h"
#include "KittensMazeWorldSettings.generated.h"

class UPostProcessComponent;
class UMaterialBouncingComponent;
class UKittensMazeFocusWidget;
class AVoxelWorld;

DECLARE_MULTICAST_DELEGATE_OneParam(FGamePausedDelegate, bool);

/**
 * 
 */
UCLASS(Blueprintable)
class KITTENSMAZE_API AKittensMazeWorldSettings : public AWorldSettings
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UMaterialBouncingComponent* MaterialBouncingComponent;

	UPROPERTY(EditAnywhere, Category = "World|Subsystems")
	bool bInitWorldSubsystems;

	UPROPERTY(EditDefaultsOnly, Category = "World|Subsystems", meta = (Categories = "ObjectPooling", EditCondition = bInitWorldSubsystems))
	TMap<FGameplayTag, FObjectPoolingInfo> PooledActorsData;

	UPROPERTY(EditAnywhere, Category = "World", meta = (Categories = "WorldActor"))
	TMap<FGameplayTag, AActor*> WorldActors;

	UPROPERTY(EditAnywhere, Category = "GameMode")
	TSubclassOf<UKittensMazeFocusWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Voxel")
	FGameplayTag VoxelEventTag;

	UPROPERTY()
	TWeakObjectPtr<UKittensMazeFocusWidget> MainMenuWidget;

public:
	FGamePausedDelegate OnGamePaused;
	
	bool GetInitWorldSubsystems() const;
	const TMap<FGameplayTag, FObjectPoolingInfo>& GetPooledActorsData() const;
	
	AActor* GetWorldActor(const FGameplayTag& ActorTag) const;
	TSet<AVoxelWorld*> GetAllVoxelWorlds() const;
	bool AreAllVoxelWorldsLoaded() const;

	void InitMainMenuWigdet(APlayerController* OwningController);
	bool ToggleMainMenuWidget(APlayerController* OwningPlayerController);
	bool ResetMainMenuWidget(UKittensMazeFocusWidget* InMainMenuWidget);

	//~ Begin AWorldSettings interface
	virtual void SetPauserPlayerState(APlayerState* PlayerState) override;
	//~ End AWorldSettings interface

	//~ Begin AActor interface
	virtual void PostInitializeComponents() override;
	//~ End AActor interface

private:
	UFUNCTION()
	void OnPointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	void InitVoxels();

	void ResetMainMenuWidgetInternal();
};
