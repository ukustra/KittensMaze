// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ImpactDataObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class KITTENSMAZE_API UImpactDataObject : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Impact")
	bool bPlayWhoosh;

	UPROPERTY(BlueprintReadOnly, Category = "Impact")
	FTransform WhooshTransform;

	UPROPERTY(BlueprintReadOnly, Category = "Impact")
	bool bHitDestructible;
};
