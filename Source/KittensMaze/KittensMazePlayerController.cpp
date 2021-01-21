// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazePlayerController.h"
#include "KittensMazeGameInstance.h"
#include "KittensMazeWorldSettings.h"
#include "UI/KittensMazeFocusWidget.h"

AKittensMazePlayerController::AKittensMazePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, BaseTurnRate(45.f)
	, BaseLookUpRate(45.f)
{

}

void AKittensMazePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis(TEXT("MoveForward"), this, &AKittensMazePlayerController::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &AKittensMazePlayerController::MoveRight);

	InputComponent->BindAxis(TEXT("Turn"), this, &APlayerController::AddYawInput);
	InputComponent->BindAxis(TEXT("TurnRate"), this, &AKittensMazePlayerController::TurnAtRate);
	InputComponent->BindAxis(TEXT("LookUp"), this, &APlayerController::AddPitchInput);
	InputComponent->BindAxis(TEXT("LookUpRate"), this, &AKittensMazePlayerController::LookUpAtRate);

	FInputActionBinding PauseBinding = FInputActionBinding(TEXT("Pause"), IE_Pressed);
	PauseBinding.bExecuteWhenPaused = true;
	PauseBinding.ActionDelegate.BindDelegate(this, &AKittensMazePlayerController::OnPausePressed);
	InputComponent->AddActionBinding(PauseBinding);
}

void AKittensMazePlayerController::MoveForward(float Value)
{
	if ((GetPawn()) && (Value != 0.0f))
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		GetPawn()->AddMovementInput(Direction, Value);
	}
}

void AKittensMazePlayerController::MoveRight(float Value)
{
	if ((GetPawn()) && (Value != 0.0f))
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		GetPawn()->AddMovementInput(Direction, Value); 
	}
}

void AKittensMazePlayerController::OnPausePressed()
{
	if (GetWorld())
	{
		if (UKittensMazeGameInstance* GameInstance = Cast<UKittensMazeGameInstance>(GetGameInstance()))
		{
			if (GameInstance->GetSaveGameFromSlot(TEXT("Settings")))
			{
				if (AKittensMazeWorldSettings* WorldSettings = Cast<AKittensMazeWorldSettings>(GetWorld()->GetWorldSettings()))
				{
					WorldSettings->ToggleMainMenuWidget(GetGameInstance()->GetFirstLocalPlayerController(GetWorld()));
				}
			}
		}
	}
}

void AKittensMazePlayerController::TurnAtRate(float Rate)
{
	AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AKittensMazePlayerController::LookUpAtRate(float Rate)
{
	AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
