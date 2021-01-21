// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BombsAmountExecution.generated.h"

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UBombsAmountExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_UCLASS_BODY()
	
	//~ Begin UGameplayEffectExecutionCalculation Interface
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	//~ End UGameplayEffectExecutionCalculation Interface
};
