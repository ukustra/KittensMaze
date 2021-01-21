// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "KittensMazeSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig)
class KITTENSMAZE_API UKittensMazeSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
	
protected:
	/** For various abilities, for instance bounce. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Damage", meta = (ClampMin = 0.f))
	float MinDamageToNormalize;

	/** For various abilities, for instance bounce. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Damage", meta = (ClampMin = 0.f))
	float MaxDamageToNormalize;

	UPROPERTY(config, EditDefaultsOnly, Category = "Damage", meta = (ClampMin = 0.f))
	float BombDamageMultiplier;

public:
	static UKittensMazeSettings* Get();

	float GetMinDamageToNormalize() const;
	float GetMaxDamageToNormalize() const;
	float GetBombDamageMultiplier() const;
};
