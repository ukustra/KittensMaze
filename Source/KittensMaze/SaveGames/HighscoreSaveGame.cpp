// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "HighscoreSaveGame.h"
#include "KittensMaze/KittensMazeGameInstance.h"

UHighscoreSaveGame::UHighscoreSaveGame(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    
}

bool UHighscoreSaveGame::RegisterHighscore(const UObject* WorldContextObject, float HighscoreCandidate)
{
    if (GEngine)
    {
        if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            if (UKittensMazeGameInstance* GameInstance = Cast<UKittensMazeGameInstance>(World->GetGameInstance()))
            {
                const int32 MaxSize = GameInstance->GetHighscoreListSize();
                if (Highscores.Num() < MaxSize)
                {
                    if (!Highscores.Contains(HighscoreCandidate))
                    {
                        Highscores.Add(HighscoreCandidate);
                        SortHighscores();

                        return true;
                    }
                }
                else
                {
                    const bool bAddHighscore = Invoke([this, HighscoreCandidate]()->bool const
                    {
                        if (Highscores.Contains(HighscoreCandidate))
                        {
                            return false;
                        }
                        
                        for (const float Highscore : Highscores)
                        {
                            if (HighscoreCandidate > Highscore)
                            {
                                return true;
                            }
                        }
                        
                        return false;
                    });

                    if (bAddHighscore)
                    {
                        Highscores.Add(HighscoreCandidate);
                    }

                    SortHighscores();
                    
                    for (int32 i = MaxSize; i < Highscores.Num(); ++i)
                    {
                        Highscores.RemoveAt(i);
                    }

                    return bAddHighscore;
                }
            }
        }
    }

    return false;
}

void UHighscoreSaveGame::SortHighscores()
{
    Highscores.Sort([](float A, float B)
    {
        return  A > B;
    });
}

float UHighscoreSaveGame::GetHighscoreAt(int32 Index) const
{
    return Highscores.IsValidIndex(Index) ? Highscores[Index] : 0.f;
}
