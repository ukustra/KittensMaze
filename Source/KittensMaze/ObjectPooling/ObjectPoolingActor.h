// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPoolingActor.generated.h"

UCLASS()
class KITTENSMAZE_API AObjectPoolingActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "ObjectPooling")
	UObject* AdditionalData;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ObjectPooling", meta = (DisplayName = "On Actor Active Changed"))
	void K2_OnActorActiveChanged(bool bIsActive);

	virtual void OnActorActiveChanged(bool bIsActive);
	
	//~ Begin AActor interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	virtual void SetLifeSpan(float InLifespan) override;
	virtual void PreInitializeComponents() override;
	//~ End AActor interface

	void SetActive(bool bInActive);
	bool IsActive() const;

	void SetAdditionalData(UObject* NewData);
	UObject* GetAdditionalData() const;

private:
	float PoolLifespan;
	FTimerHandle LifespanTimer;
	bool bActive;
};
