// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazeFocusWidget.h"
#include "GameFramework/InputSettings.h"
#include "KittensMaze/KittensMazeWorldSettings.h"

UKittensMazeFocusWidget::UKittensMazeFocusWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    
}

void UKittensMazeFocusWidget::InitFocusWidget(UWidget* Widget)
{
    FocusWidget = Widget;
    if (FocusWidget)
    {
        FocusWidget->SetFocus();
    }
}

void UKittensMazeFocusWidget::OnBackButtonPressed_Implementation()
{
    RemoveFromParent();

    if (const UWorld* World = GetWorld())
    {
        if (AKittensMazeWorldSettings* WorldSettings = Cast<AKittensMazeWorldSettings>(World->GetWorldSettings()))
        {
            WorldSettings->InitMainMenuWigdet(GetOwningPlayer());
        }
    }
}

bool UKittensMazeFocusWidget::CanBeRemoved_Implementation() const
{
    return true;
}

void UKittensMazeFocusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (FocusWidget)
    {
        const FVector2D NewWindowSize = MyGeometry.GetAbsoluteSize();
        if (!CurrentWindowSize.Equals(NewWindowSize))
        {
            CurrentWindowSize = NewWindowSize;
            FocusWidget->SetFocus();
        }
    }
}

void UKittensMazeFocusWidget::NativeDestruct()
{
    if (const UWorld* World = GetWorld())
    {
        if (AKittensMazeWorldSettings* WorldSettings = Cast<AKittensMazeWorldSettings>(World->GetWorldSettings()))
        {
            WorldSettings->ResetMainMenuWidget(this);
        }
    }

    Super::NativeDestruct();
}

FReply UKittensMazeFocusWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (FocusWidget)
    {
        FocusWidget->SetFocus();
        return FReply::Unhandled();
    }

    return  Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UKittensMazeFocusWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (const UInputSettings* InputSettings = UInputSettings::GetInputSettings())
    {
        TArray<FInputActionKeyMapping> BackMappings;
        InputSettings->GetActionMappingByName(TEXT("Back"), BackMappings);

        for (const FInputActionKeyMapping& Mapping : BackMappings)
        {
            if (Mapping.Key == InKeyEvent.GetKey())
            {
                OnBackButtonPressed();
                break;
            }
        }
    }

    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}
