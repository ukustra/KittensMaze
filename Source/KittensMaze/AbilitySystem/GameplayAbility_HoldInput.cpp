// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "GameplayAbility_HoldInput.h"
#include "AbilitySystemComponent.h"

UGameplayAbility_HoldInput::UGameplayAbility_HoldInput(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_HoldInput::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (ActorInfo && AbilitiesToActivateOnHold.IsValid())
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);

		ActivateHoldAbility();
		ActorInfo->AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &UGameplayAbility_HoldInput::OnHoldAbilityEnded);
	}
	else
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateEnd);
	}
}

void UGameplayAbility_HoldInput::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->OnAbilityEnded.RemoveAll(this);

		if (bWasCancelled)
		{
			ActorInfo->AbilitySystemComponent->CancelAbilities(&AbilitiesToActivateOnHold);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_HoldInput::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEnd, false);
}

void UGameplayAbility_HoldInput::ActivateHoldAbility()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->TryActivateAbilitiesByTag(AbilitiesToActivateOnHold);
	}
}

void UGameplayAbility_HoldInput::OnHoldAbilityEnded(const FAbilityEndedData& EndData)
{
	if (GetWorld() && EndData.AbilityThatEnded && EndData.AbilityThatEnded->AbilityTags.HasAny(AbilitiesToActivateOnHold))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UGameplayAbility_HoldInput::ActivateHoldAbility);
	}
}
