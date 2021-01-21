// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_HoldInput.generated.h"

/**
 * 
 */
UCLASS()
class KITTENSMAZE_API UGameplayAbility_HoldInput : public UGameplayAbility
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HoldInput")
	FGameplayTagContainer AbilitiesToActivateOnHold;

	//~ Begin GameplayAbilities Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
public:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~ End GameplayAbilities Interface

private:
	bool bReplicateEnd;

	void ActivateHoldAbility();
	void OnHoldAbilityEnded(const FAbilityEndedData& EndData);
};
