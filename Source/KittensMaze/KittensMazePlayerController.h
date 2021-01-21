// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KittensMazePlayerController.generated.h"

class UKittensMazeFocusWidget;

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API AKittensMazePlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

protected:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	//~ Begin APlayerController interface
	virtual void SetupInputComponent() override;
	//~ End APlayerController interface	

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnPausePressed();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
};
