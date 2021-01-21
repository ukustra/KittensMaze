// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VoxelWorld.h"
#include "GameFramework/GameModeBase.h"
#include "KittensMazeGameMode.generated.h"

class AKittensMazeWorldSettings;

UCLASS(minimalapi)
class AKittensMazeGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UAudioComponent* MusicAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game", meta = (ClampMin = 0.f))
	float MinutesOfGame;

	UFUNCTION(BlueprintImplementableEvent, Category = "Game", meta = (DisplayName = "On All Players Restarted"))
	void K2_OnAllPlayersRestarted();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Audio")
	void FadePauseMusicIn();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Audio")
    void FadePauseMusicOut();

	//~ Begin AGameModeBase Interface
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
public:
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	//~ End AGameModeBase Interface

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	float GetMinutesOfGame() const;

private:
	UPROPERTY()
	AKittensMazeWorldSettings* WorldSettings;

	UPROPERTY()
	TSet<AActor*> TakenPlayerStarts;

	UPROPERTY()
	TSet<AController*> PlayersToRestart;

	UFUNCTION()
	void OnVoxelWorldLoaded();
};



