// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazeGameInstance.h"
#include "MoviePlayer.h"
#include "Sound/SoundClass.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGames/SettingsSaveGame.h"

UKittensMazeGameInstance::UKittensMazeGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, HighscoreListSize(10)
	, NumberOfLocalPlayers(1)
{
	
}

bool UKittensMazeGameInstance::IsStringCharacterPreset(const FString& String) const
{
	return CharacterPresets.Contains(String);
}

USaveGame* UKittensMazeGameInstance::GetSaveGameFromSlot(const FString& Slot) const
{
	return SaveGameObjects.Contains(Slot) ? SaveGameObjects[Slot] : nullptr;
}

void UKittensMazeGameInstance::SetNumberOfLocalPlayers(int32 InLocalPlayers)
{
	NumberOfLocalPlayers = FMath::Max(InLocalPlayers, 1);
}

void UKittensMazeGameInstance::SetSoundClassVolume(const FString& InSoundClassName, float NewVolume, bool bSaveVolume)
{
	if (USoundClass* SoundClass = SoundClasses.FindRef(InSoundClassName))
	{
		const float ClampedVolume = FMath::Max(NewVolume, 0.f);
		if (SoundClass->Properties.Volume != ClampedVolume)
		{
			SoundClass->Properties.Volume = ClampedVolume;

			if (SettingsSaveGame)
			{
				SettingsSaveGame->SetSoundClassVolume(InSoundClassName, ClampedVolume);
				if (bSaveVolume)
				{
					UGameplayStatics::SaveGameToSlot(SettingsSaveGame, SettingsSaveSlot, UserIndex);
				}
			}
		}
	}
}

FCharacterInfo UKittensMazeGameInstance::GetCharacterPreset(const FString& CharacterName) const
{
	return CharacterPresets.Contains(CharacterName) ? CharacterPresets[CharacterName] : FCharacterInfo();
}

int32 UKittensMazeGameInstance::GetHighscoreListSize() const
{
	return FMath::Max(HighscoreListSize,0);
}

int32 UKittensMazeGameInstance::GetNumberOfLocalPlayers() const
{
	return FMath::Max(NumberOfLocalPlayers, 1);
}

TArray<FString> UKittensMazeGameInstance::GetChoosenCharacters() const
{
	return ChoosenCharacters;
}

void UKittensMazeGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UKittensMazeGameInstance::ShowLoadingScreen);

	for (const auto& It : SaveGameClasses)
	{
		if (const TSubclassOf<USaveGame>& SaveGameClass = It.Value)
		{
			USaveGame* SaveGameObject = nullptr;
			if (UGameplayStatics::DoesSaveGameExist(It.Key, UserIndex))
			{
				SaveGameObject = UGameplayStatics::LoadGameFromSlot(It.Key, UserIndex);
			}
			else
			{
				SaveGameObject = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
			}

			if (SaveGameObject)
			{
				SaveGameObjects.Add(It.Key, SaveGameObject);
			}

			if (!SettingsSaveGame)
			{
				SettingsSaveGame = Cast<USettingsSaveGame>(SaveGameObject);
				if (SettingsSaveGame)
				{
					SettingsSaveSlot = It.Key;
					if (SettingsSaveGame->bFirstLaunch)
					{
						const FString Command = TEXT("SCALABILITY AUTO");
						if (APlayerController* PlayerController = GetFirstLocalPlayerController())
						{
							PlayerController->ConsoleCommand(Command, true);
						}
						else if (GEngine)
						{
							GEngine->Exec(GetWorld(), *Command);
						}
					}
					
					for (const auto& SoundIt : SoundClasses)
					{
						const FString& SoundClassName = SoundIt.Key;
						SettingsSaveGame->InitSoundClass(SoundClassName);
						
						const float ClassVolume = SettingsSaveGame->GetSoundClassVolume(SoundClassName);
						SetSoundClassVolume(SoundClassName, ClassVolume, false);
					}
				}
			}
		}
	}
}

void UKittensMazeGameInstance::Shutdown()
{
	if (SettingsSaveGame && SettingsSaveGame->bFirstLaunch)
	{
		SettingsSaveGame->bFirstLaunch = false;
	}
	
	for (const auto& It : SaveGameObjects)
	{
		if (USaveGame* SaveGameObject = It.Value)
		{
			UGameplayStatics::SaveGameToSlot(SaveGameObject, It.Key, UserIndex);
		}
	}

	Super::Shutdown();
}

void UKittensMazeGameInstance::ShowLoadingScreen(const FString& MapName)
{
	const FLoadingScreenAttributes LoadingScreen = Invoke([]()->FLoadingScreenAttributes const
        {
            FLoadingScreenAttributes LoadingScreenAttributes;
            LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = false;
            LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

            return LoadingScreenAttributes;
        });

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}
