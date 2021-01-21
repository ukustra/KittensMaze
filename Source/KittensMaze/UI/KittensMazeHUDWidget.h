// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "KittensMazeHUDWidget.generated.h"

class UKittensMazeAttributeWidget;

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UKittensMazeHUDWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	UKittensMazeAttributeWidget* GetCoinsWidget() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	UKittensMazeAttributeWidget* GetBombsWidget() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	UKittensMazeAttributeWidget* GetBoostWidget() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetAttributeWidgetValue(const FGameplayAttribute& Attribute, float NewValue, bool bPlayAnim);

	void Init();
	
private:
	UPROPERTY()
	TMap<FGameplayAttribute, UKittensMazeAttributeWidget*> AttributeWidgets;

	void InitAttributeWidget(const FGameplayAttribute& Attribute, UKittensMazeAttributeWidget* AttributeWidget);
};
