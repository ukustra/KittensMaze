// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KittensMazeAttributeWidget.generated.h"

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UKittensMazeAttributeWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "UI")
	void SetAttributeText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "UI")
	void PlayAnim();
};
