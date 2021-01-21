// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "InventoryAttributeSet.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class KITTENSMAZE_API UInventoryAttributeSet final : public UAttributeSet
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	FGameplayAttributeData CoinsAmount;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UInventoryAttributeSet, CoinsAmount) // Macro from AttributeSet.h

	UPROPERTY()
	FGameplayAttributeData BombsAmount;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UInventoryAttributeSet, BombsAmount)
	
	UPROPERTY()
	FGameplayAttributeData BoostAmount;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UInventoryAttributeSet, BoostAmount)
};
