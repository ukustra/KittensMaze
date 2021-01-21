// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "ObjectPoolingActor.h"

AObjectPoolingActor::AObjectPoolingActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AObjectPoolingActor::OnActorActiveChanged(bool bIsActive)
{
	K2_OnActorActiveChanged(bIsActive);
}

void AObjectPoolingActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(LifespanTimer);
}

void AObjectPoolingActor::SetLifeSpan(float InLifespan)
{
	PoolLifespan = InLifespan;

	if (PoolLifespan > 0.f)
	{
		SetActive(true);
		GetWorldTimerManager().SetTimer(LifespanTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
        {
        	SetActive(false);
		}), PoolLifespan, false);
	}
	else
	{
		SetActive(false);
	}
}

void AObjectPoolingActor::PreInitializeComponents()
{
	for (auto& Component : GetComponents())
	{
		if (Component)
		{
			Component->SetAutoActivate(false);
		}
	}

	Super::PreInitializeComponents();
}

void AObjectPoolingActor::SetActive(bool bInActive)
{
	if (bActive != bInActive)
	{
		bActive = bInActive;
		SetActorEnableCollision(bActive);
		SetActorHiddenInGame(!bActive);

		if (!bActive)
		{
			GetWorldTimerManager().ClearTimer(LifespanTimer);
		}

		OnActorActiveChanged(bActive);
	}
}

bool AObjectPoolingActor::IsActive() const
{
	return bActive;
}

void AObjectPoolingActor::SetAdditionalData(UObject* NewData)
{
	AdditionalData = NewData;
}

UObject* AObjectPoolingActor::GetAdditionalData() const
{
	return AdditionalData;
}
