// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "CoinsAmountExecution.h"
#include "InventoryAttributeSet.h"

struct FCoinsAmountStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CoinsAmount);

	FCoinsAmountStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UInventoryAttributeSet, CoinsAmount, Target, false);
	}
};

static const FCoinsAmountStatics& CoinsAmountStatics()
{
	static FCoinsAmountStatics CoinsAmountStatics;
	return CoinsAmountStatics;
}

UCoinsAmountExecution::UCoinsAmountExecution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(CoinsAmountStatics().CoinsAmountDef);
}

void UCoinsAmountExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	const FAggregatorEvaluateParameters EvaluationParameters = Invoke([SourceTags, TargetTags]()->FAggregatorEvaluateParameters const {
		FAggregatorEvaluateParameters Params;
		Params.SourceTags = SourceTags;
		Params.TargetTags = TargetTags;

		return Params;
	});

	const float CoinsAmount = Invoke([ExecutionParams, EvaluationParameters]()->float const {
		float Coins;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CoinsAmountStatics().CoinsAmountDef, EvaluationParameters, Coins);

		return FMath::Max(0.f, Coins);
	});

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(CoinsAmountStatics().CoinsAmountProperty, EGameplayModOp::Override, CoinsAmount));
}
