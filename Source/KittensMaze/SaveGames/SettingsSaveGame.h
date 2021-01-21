// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingsSaveGame.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable)
class KITTENSMAZE_API USettingsSaveGame final : public USaveGame
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FString ScalabilityLevel;

	UPROPERTY()
	bool bFirstLaunch;

	UFUNCTION(BlueprintPure, Category = "Save")
	float GetSoundClassVolume(const FString& SoundClassName) const;

	void InitSoundClass(const FString& SoundClassName);
	void SetSoundClassVolume(const FString& SoundClassName, float NewVolume);

private:
	UPROPERTY()
	TMap<FString, float> SoundClassVolumes;
};
