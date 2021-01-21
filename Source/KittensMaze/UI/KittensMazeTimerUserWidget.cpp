// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazeTimerUserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "KittensMaze/KittensMazeGameMode.h"
#include "KittensMaze/KittensMazeWorldSettings.h"

UKittensMazeTimerUserWidget::UKittensMazeTimerUserWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    
}

void UKittensMazeTimerUserWidget::InitTextWidget(UTextBlock* InTextWidge)
{
    TextWidget = InTextWidge;
}

void UKittensMazeTimerUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (const UWorld* World = GetWorld())
    {
        if (AKittensMazeWorldSettings* WorldSettings = Cast<AKittensMazeWorldSettings>(World->GetWorldSettings()))
        {
            WorldSettings->OnGamePaused.AddUObject(this, &UKittensMazeTimerUserWidget::OnGamePaused);
        }
        
        if (const AKittensMazeGameMode* GameMode = Cast<AKittensMazeGameMode>(World->GetAuthGameMode()))
        {
            TimeOfGame = GameMode->GetMinutesOfGame() * 60.f;
        }
    }
}

void UKittensMazeTimerUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    TimeSinceCreation += InDeltaTime;

    if (TextWidget)
    {
        const int32 TimeRemaining = FMath::FloorToInt(TimeOfGame - TimeSinceCreation);

        if (TimeRemaining > 0)
        {
            const int32 MinutesCandidate = TimeRemaining / 60;
            const int32 Minutes = MinutesCandidate >= 60 ? MinutesCandidate % 60 : MinutesCandidate;
            const int32 Seconds = TimeRemaining % 60;

            if (Minutes == 0 && Seconds == 30)
            {
                if (!TextWidget->ColorAndOpacity.GetSpecifiedColor().Equals(LowTimeColor))
                {
                    TextWidget->SetColorAndOpacity(FSlateColor(LowTimeColor));
                    PlayBeepSound(Seconds);
                }
            }

            if (Minutes == 0 && Seconds <= 10)
            {
                PlayBeepSound(Seconds);
            }

            const FString TimerString = GetTimeString(Minutes) + TEXT(":") + GetTimeString(Seconds);
            const FText TimerText = FText::AsCultureInvariant(TimerString);
            if (!TextWidget->GetText().EqualTo(TimerText))
            {
                TextWidget->SetText(TimerText);
            }
        }
        else
        {
            bTimerFinished = true;
            RemoveFromParent();
        }
    }
}

FString UKittensMazeTimerUserWidget::GetTimeString(int32 Time) const
{
    return Time >= 10 ? FString::FromInt(Time) : TEXT("0") + FString::FromInt(Time);
}

void UKittensMazeTimerUserWidget::OnGamePaused(bool bGamePaused)
{
    if (bGamePaused)
    {
        if (IsVisible())
        {
            SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    else if (!IsVisible())
    {
        SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

bool UKittensMazeTimerUserWidget::PlayBeepSound(int32 InSeconds)
{
    if (BeepTime != InSeconds)
    {
        UGameplayStatics::PlaySound2D(GetOwningPlayerPawn(), BeepSound);
        BeepTime = InSeconds;

        return true;
    }

    return false;
}
