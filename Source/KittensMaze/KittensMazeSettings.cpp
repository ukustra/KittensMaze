// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazeSettings.h"

UKittensMazeSettings::UKittensMazeSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MinDamageToNormalize(5.f)
	, MaxDamageToNormalize(50.f)
	, BombDamageMultiplier(1.5f)
{

}

UKittensMazeSettings* UKittensMazeSettings::Get()
{
	return UKittensMazeSettings::StaticClass()->GetDefaultObject<UKittensMazeSettings>();
}

float UKittensMazeSettings::GetMinDamageToNormalize() const
{
	return FMath::Max(MinDamageToNormalize, 0.f);
}

float UKittensMazeSettings::GetMaxDamageToNormalize() const
{
	return FMath::Max(MaxDamageToNormalize, 0.f);
}

float UKittensMazeSettings::GetBombDamageMultiplier() const
{
	return FMath::Max(BombDamageMultiplier, 0.f);
}
