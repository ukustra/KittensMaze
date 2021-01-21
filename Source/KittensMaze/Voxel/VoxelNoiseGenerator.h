// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VoxelGenerators/VoxelGenerator.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "FastNoise/VoxelFastNoise.h"
#include "VoxelNoiseGenerator.generated.h"

/**
 * @see https://wiki.voxelplugin.com/World_Generators#Creating_a_new_C.2B.2B_World_Generator
 * @see UVoxelGeneratorExample
 */
UCLASS(Blueprintable)
class UVoxelNoiseGenerator : public UVoxelGenerator
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Generator", meta = (ClampMin = 0.f))
	float NoiseHeight;

	//~ Begin UVoxelGenerator Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UVoxelGenerator Interface
};

class FVoxelNoiseGeneratorInstance : public TVoxelGeneratorInstanceHelper<FVoxelNoiseGeneratorInstance, UVoxelNoiseGenerator>
{
public:
	using Super = TVoxelGeneratorInstanceHelper<FVoxelNoiseGeneratorInstance, UVoxelNoiseGenerator>;

	explicit FVoxelNoiseGeneratorInstance(UVoxelNoiseGenerator& InGenerator);

	//~ Begin FVoxelGeneratorInstance Interface
	virtual void Init(const FVoxelGeneratorInit& InitStruct) override;

	v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;
	FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;

	TVoxelRange<v_flt> GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const;

	virtual FVector GetUpVector(v_flt X, v_flt Y, v_flt Z) const override final;
	//~ End FVoxelGeneratorInstance Interface

private:
	const float NoiseHeight;
	FVoxelFastNoise Noise;
};
