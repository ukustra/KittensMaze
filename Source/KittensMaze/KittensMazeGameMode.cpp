// Copyright 2021 Urszula Kustra. All Rights Reserved.

#include "KittensMazeGameMode.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "VoxelWorld.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Components/AudioComponent.h"
#include "KittensMazeGameMode.h"
#include "KittensMazeCharacter.h"
#include "KittensMazeWorldSettings.h"
#include "KittensMazeGameInstance.h"

AKittensMazeGameMode::AKittensMazeGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MinutesOfGame(3.f)
{
	MusicAudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("MusicAudioComponent"));
	MusicAudioComponent->bAllowSpatialization = false;
	MusicAudioComponent->SetUISound(true);
	MusicAudioComponent->bAutoActivate = false;
	SetRootComponent(MusicAudioComponent);

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AKittensMazeGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);

	PlayersToRestart.Remove(NewPlayer);

	if (PlayersToRestart.Num() == 0 && GetWorld() && GetWorld()->GetGameState())
	{
		TakenPlayerStarts.Empty();

		for (APlayerState* PlayerState : GetWorld()->GetGameState()->PlayerArray)
		{
			if (AKittensMazeCharacter* PlayerCharacter = Cast<AKittensMazeCharacter>(PlayerState->GetPawn()))
			{
				PlayerCharacter->K2_OnAllPlayersRestarted();
			}
		}

		K2_OnAllPlayersRestarted();
	}
}

void AKittensMazeGameMode::RestartPlayer(AController* NewPlayer)
{
	if (GetWorld())
	{
		WorldSettings = Cast<AKittensMazeWorldSettings>(GetWorld()->GetWorldSettings());
		if (WorldSettings)
		{
			if (!WorldSettings->AreAllVoxelWorldsLoaded())
			{
				PlayersToRestart.Add(NewPlayer);
				
				for (auto& VoxelWorld : WorldSettings->GetAllVoxelWorlds())
				{
					if (VoxelWorld)
					{
						VoxelWorld->OnWorldLoaded.AddUniqueDynamic(this, &AKittensMazeGameMode::OnVoxelWorldLoaded);
					}
				}

				return;
			}
		}
	}

	Super::RestartPlayer(NewPlayer);
}

AActor* AKittensMazeGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	AActor* PlayerStartCandidate = Super::FindPlayerStart_Implementation(Player, IncomingName);

	if (PlayerStartCandidate && !TakenPlayerStarts.Contains(PlayerStartCandidate))
	{
		TakenPlayerStarts.Add(PlayerStartCandidate);
		return PlayerStartCandidate;
	}
	else
	{
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			APlayerStart* Start = *It;
			if (Start && !TakenPlayerStarts.Contains(Start))
			{
				TakenPlayerStarts.Add(Start);
				return Start;
			}
		}
	}

	return nullptr;
}

void AKittensMazeGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		if ( UKittensMazeGameInstance* GameInstance = Cast<UKittensMazeGameInstance>(GetWorld()->GetGameInstance()) )
		{
			for (int32 i = 0; i < GameInstance->GetNumberOfLocalPlayers() - 1; ++i)
			{
				FString Error;
				GameInstance->CreateLocalPlayer(-1, Error, true);

				if (Error.Len() > 0)
				{
					UE_LOG(LogPlayerManagement, Error, TEXT("Failed to Create Player: %s"), *Error);
				}
			}
		}
	}
}

float AKittensMazeGameMode::GetMinutesOfGame() const
{
	return MinutesOfGame;
}

void AKittensMazeGameMode::OnVoxelWorldLoaded()
{
	if (WorldSettings && WorldSettings->AreAllVoxelWorldsLoaded())
	{
		const TSet<AController*> Players = PlayersToRestart;
		for (auto& Player : Players)
		{
			if (Player)
			{
				Super::RestartPlayer(Player);
			}
		}
	}
}
