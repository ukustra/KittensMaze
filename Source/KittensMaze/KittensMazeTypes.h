// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "ObjectPooling/ObjectPoolingActor.h"
#include "KittensMazeTypes.generated.h"

USTRUCT(BlueprintType)
struct FCharacterInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* Eyes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* WeaponWhooshSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "VO"))
	TMap<FGameplayTag, USoundBase*> Voices;

	/** Attached to the Character Mesh. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FName, UStaticMesh*> AdditionalMeshes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f))
	float AttackMontageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f))
	float Damage = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f))
	float JumpPower = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f))
	float GravityScale = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor WidgetColor = FLinearColor::White;

	bool IsValid() const
	{
		return Material && Eyes && WeaponMesh;
	}
};

USTRUCT(BlueprintType)
struct FObjectPoolingInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AObjectPoolingActor> ActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f))
	float LifeSpan = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0))
	int32 PoolSize = 5;

	bool IsValid() const
	{
		return ActorClass != nullptr && LifeSpan > 0.f;
	}
};
