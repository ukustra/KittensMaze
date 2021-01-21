// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KittensMazeAnimInstance.generated.h"

class AKittensMazeCharacter;

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UKittensMazeAnimInstance : public UAnimInstance
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	AKittensMazeCharacter* Character;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Speed;

public:
	//~ Begin UAnimInstance Interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	//~ End UAnimInstance Interface
};
