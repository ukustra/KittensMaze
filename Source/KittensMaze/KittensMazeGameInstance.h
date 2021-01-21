// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KittensMazeTypes.h"
#include "KittensMazeGameInstance.generated.h"

class USaveGame;
class USettingsSaveGame;

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UKittensMazeGameInstance : public UGameInstance
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Save")
	TMap<FString, TSubclassOf<USaveGame>> SaveGameClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = 0))
	int32 HighscoreListSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = 1))
	int32 NumberOfLocalPlayers;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TMap<FString, USoundClass*> SoundClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TMap<FString, FCharacterInfo> CharacterPresets;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TArray<FString> ChoosenCharacters;

public:
	UFUNCTION(BlueprintPure, Category = "Settings")
	bool IsStringCharacterPreset(const FString& String) const;

	UFUNCTION(BlueprintPure, Category = "Settings")
	USaveGame* GetSaveGameFromSlot(const FString& Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetNumberOfLocalPlayers(int32 InLocalPlayers);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetSoundClassVolume(const FString& InSoundClassName, float NewVolume, bool bSaveVolume = true);

	FCharacterInfo GetCharacterPreset(const FString& CharacterName) const;
	int32 GetHighscoreListSize() const;
	int32 GetNumberOfLocalPlayers() const;
	TArray<FString> GetChoosenCharacters() const;

	//~ Begin UGameInstance Interface
	virtual void Init() override;
	virtual void Shutdown() override;
	//~ End UGameInstance Interface

private:
	UPROPERTY()
	TMap<FString, USaveGame*> SaveGameObjects;

	UPROPERTY()
	USettingsSaveGame* SettingsSaveGame;

	FString SettingsSaveSlot;
	int32 UserIndex;

	void ShowLoadingScreen(const FString& MapName);
};
