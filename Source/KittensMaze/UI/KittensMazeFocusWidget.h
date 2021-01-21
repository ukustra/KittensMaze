// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KittensMazeFocusWidget.generated.h"

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UKittensMazeFocusWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitFocusWidget(UWidget* Widget);

	UFUNCTION(BlueprintNativeEvent, Category = "UI")
	void OnBackButtonPressed();
	
	//~ Begin UUserWidget interface
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//~ End UUserWidget interface

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "UI")
    bool CanBeRemoved() const;
	
private:
	UPROPERTY()
	UWidget* FocusWidget;

	FVector2D CurrentWindowSize;
};
