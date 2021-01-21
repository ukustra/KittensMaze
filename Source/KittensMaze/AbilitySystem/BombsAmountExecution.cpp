// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "BombsAmountExecution.h"
#include "InventoryAttributeSet.h"

struct FBombsAmountStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BombsAmount);

	FBombsAmountStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UInventoryAttributeSet, BombsAmount, Target, false);
	}
};

static const FBombsAmountStatics& BombsAmountStatics()
{
	static FBombsAmountStatics BombsAmountStatics;
	return BombsAmountStatics;
}

UBombsAmountExecution::UBombsAmountExecution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(BombsAmountStatics().BombsAmountDef);
}

void UBombsAmountExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	const float BombsAmount = Invoke([ExecutionParams, EvaluationParameters]()->float const {
		float Bombs;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BombsAmountStatics().BombsAmountDef, EvaluationParameters, Bombs);

		return FMath::Max(0.f, Bombs);
	});

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(BombsAmountStatics().BombsAmountProperty, EGameplayModOp::Override, BombsAmount));
}
