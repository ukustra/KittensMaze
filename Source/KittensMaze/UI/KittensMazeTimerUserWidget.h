// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KittensMazeTimerUserWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UKittensMazeTimerUserWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FLinearColor LowTimeColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	USoundBase* BeepSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "UI")
	bool bTimerFinished;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitTextWidget(UTextBlock* InTextWidge);
	
	//~ Begin UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget interface

private:
	UPROPERTY()
	UTextBlock* TextWidget;
	
	float TimeSinceCreation;
	float TimeOfGame;
	int32 BeepTime;

	FString GetTimeString(int32 Time) const;
	void OnGamePaused(bool bGamePaused);
	bool PlayBeepSound(int32 InSeconds);
};
