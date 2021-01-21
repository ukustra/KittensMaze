// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "BoostAmountExecution.h"
#include "InventoryAttributeSet.h"

struct FBoostAmountStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BoostAmount);

	FBoostAmountStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UInventoryAttributeSet, BoostAmount, Target, false);
	}
};

static const FBoostAmountStatics& BoostAmountStatics()
{
	static FBoostAmountStatics BoostAmountStatics;
	return BoostAmountStatics;
}

UBoostAmountExecution::UBoostAmountExecution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(BoostAmountStatics().BoostAmountDef);
}

void UBoostAmountExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	const float BoostAmount = Invoke([ExecutionParams, EvaluationParameters]()->float const {
		float Boost;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BoostAmountStatics().BoostAmountDef, EvaluationParameters, Boost);

		return FMath::Max(0.f, Boost);
	});

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(BoostAmountStatics().BoostAmountProperty, EGameplayModOp::Override, BoostAmount));
}
