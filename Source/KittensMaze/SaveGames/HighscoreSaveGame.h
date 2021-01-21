// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HighscoreSaveGame.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable)
class KITTENSMAZE_API UHighscoreSaveGame final : public USaveGame
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Save", meta = (WorldContext = "WorldContextObject"))
	bool RegisterHighscore(const UObject* WorldContextObject, float HighscoreCandidate);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SortHighscores();

	UFUNCTION(BlueprintPure, Category = "Save")
	float GetHighscoreAt(int32 Index) const;

private:
	UPROPERTY()
	TArray<float> Highscores;
};
