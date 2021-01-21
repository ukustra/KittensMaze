// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "SettingsSaveGame.h"

USettingsSaveGame::USettingsSaveGame(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ScalabilityLevel(TEXT("AUTO"))
	, bFirstLaunch(true)
{
	
}

float USettingsSaveGame::GetSoundClassVolume(const FString& SoundClassName) const
{
    if (!bFirstLaunch && SoundClassVolumes.Contains(SoundClassName))
	{
		return SoundClassVolumes[SoundClassName];
	}

	return 1.f;
}

void USettingsSaveGame::InitSoundClass(const FString& SoundClassName)
{
	if (!SoundClassVolumes.Contains(SoundClassName))
	{
		SetSoundClassVolume(SoundClassName, 1.f);
	}
}

void USettingsSaveGame::SetSoundClassVolume(const FString& SoundClassName, float NewVolume)
{
	SoundClassVolumes.Add(SoundClassName, NewVolume);
}
