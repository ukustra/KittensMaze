// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "VoxelNoiseGenerator.h"
#include "FastNoise/VoxelFastNoise.inl"

UVoxelNoiseGenerator::UVoxelNoiseGenerator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NoiseHeight(10.f)
{

}

TVoxelSharedRef<FVoxelGeneratorInstance> UVoxelNoiseGenerator::GetInstance()
{
	return MakeVoxelShared<FVoxelNoiseGeneratorInstance>(*this);
}

FVoxelNoiseGeneratorInstance::FVoxelNoiseGeneratorInstance(UVoxelNoiseGenerator& InGenerator)
	: Super(&InGenerator)
	, NoiseHeight(InGenerator.NoiseHeight)
{
}

void FVoxelNoiseGeneratorInstance::Init(const FVoxelGeneratorInit& InitStruct)
{
	static const FName SeedName = TEXT("MySeed");
	Noise.SetSeed(InitStruct.Seeds.Contains(SeedName) ? InitStruct.Seeds[SeedName] : 1337);
}

v_flt FVoxelNoiseGeneratorInstance::GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	const float Height = Noise.GetPerlin_2D(X, Y, 0.01f) * NoiseHeight;

	// Positive value -> empty voxel
	// Negative value -> full voxel
	// Value positive when Z > Height, and negative Z < Height
	float Value = Z - Height;

	// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
	Value /= 5;

	return Value;
}

FVoxelMaterial FVoxelNoiseGeneratorInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	return FVoxelMaterial::CreateFromColor(FColor::Red);
}

TVoxelRange<v_flt> FVoxelNoiseGeneratorInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
{
	// Return the values that GetValueImpl can return in Bounds
	// Used to skip chunks where the value does not change
	// Be careful, if wrong your world will have holes!
	// By default return infinite range to be safe
	return TVoxelRange<v_flt>::Infinite();

	// Example for the GetValueImpl

	// Noise is between -1 and 1
	//const TVoxelRange<v_flt> Height = TVoxelRange<v_flt>(-1, 1) * NoiseHeight;

	// Z can go from min to max
	//TVoxelRange<v_flt> Value = TVoxelRange<v_flt>(Bounds.Min.Z, Bounds.Max.Z) - Height;

	//Value /= 5;

	//return Value;
}

FVector FVoxelNoiseGeneratorInstance::GetUpVector(v_flt X, v_flt Y, v_flt Z) const
{
	// Used by spawners
	return FVector::UpVector;
}
