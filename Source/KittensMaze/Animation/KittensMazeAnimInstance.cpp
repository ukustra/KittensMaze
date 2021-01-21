// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazeAnimInstance.h"
#include "../KittensMazeCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

UKittensMazeAnimInstance::UKittensMazeAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UKittensMazeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AKittensMazeCharacter>(TryGetPawnOwner());
}

void UKittensMazeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character)
	{
		Speed = Character->GetVelocity().Size();

		if (const UPawnMovementComponent* MovementComponent = Character->GetMovementComponent())
		{
			bIsInAir = MovementComponent->IsFalling();
		}
	}
}
