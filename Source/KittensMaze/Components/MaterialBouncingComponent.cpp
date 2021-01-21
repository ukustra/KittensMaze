// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "MaterialBouncingComponent.h"
#include "KittensMaze/KittensMazeSettings.h"

UMaterialBouncingComponent::UMaterialBouncingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PositionParamName(TEXT("PositionParam"))
	, ImpactParamName(TEXT("ImpactParam"))
	, RadiusParamName(TEXT("RadiusParam"))
	, MinRadius(100.f)
	, MaxRadius(500.f)
	, BounceMultiplier(25.f)
	, PlayRate(1.f)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bAutoActivate = false;
}

void UMaterialBouncingComponent::InitDynamicMaterial(UMaterialInstanceDynamic* DynamicMaterial)
{
	DynamicMats.Add(DynamicMaterial);
}

void UMaterialBouncingComponent::InitDynamicMaterials(const TSet<UMaterialInstanceDynamic*>& DynamicMaterials)
{
	for (auto& Material : DynamicMaterials)
	{
		if (Material)
		{
			DynamicMats.Add(Material);
		}
	}
}

void UMaterialBouncingComponent::InitDynamicMaterialsFromMesh(UMeshComponent* MeshComponent)
{
	if (MeshComponent)
	{
		DynamicMats.Empty();
		const TArray<UMaterialInterface*>& Materials = MeshComponent->GetMaterials();

		for (int32 i = 0; i < Materials.Num(); ++i)
		{
			if (UMaterialInstanceDynamic* DynamicMat = MeshComponent->CreateDynamicMaterialInstance(i, Materials[i]))
			{
				DynamicMats.Add(DynamicMat);
			}
		}
	}
}

void UMaterialBouncingComponent::Bounce(float Damage, const FVector& ImpactLocation, const FVector& ImpactNormal)
{
	if (BouncingCurve && IsSetValid())
	{
		if (UKittensMazeSettings* Settings = UKittensMazeSettings::Get())
		{
			CurvePosition = CurveMin;
			BounceNormal = ImpactNormal.GetSafeNormal();

			const float BounceRadius = FMath::GetMappedRangeValueClamped(FVector2D(Settings->GetMinDamageToNormalize(), Settings->GetMaxDamageToNormalize()), FVector2D(MinRadius, MaxRadius), Damage);

			for (auto& Material : DynamicMats)
			{
				if (Material)
				{
					Material->SetScalarParameterValue(RadiusParamName, BounceRadius);

					Material->SetVectorParameterValue(PositionParamName, FLinearColor(ImpactLocation));
					SetImpactParamValue(Material);
				}
			}

			SetComponentTickEnabled(true);

			OnBounceStarted.Broadcast(BounceRadius);
		}
	}
}

void UMaterialBouncingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (BouncingCurve)
	{
		BouncingCurve->GetTimeRange(CurveMin, CurveMax);
	}
}

void UMaterialBouncingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (BouncingCurve && IsSetValid() && CurvePosition < CurveMax)
	{
		CurvePosition = FMath::Min(CurvePosition + (DeltaTime * PlayRate), CurveMax);

		for (auto& Material : DynamicMats)
		{
			SetImpactParamValue(Material);
		}
	}
	else
	{
		SetComponentTickEnabled(false);
	}
}

bool UMaterialBouncingComponent::IsSetValid() const
{
	for (auto& Material : DynamicMats)
	{
		if (Material)
		{
			return true;
		}
	}

	return false;
}

void UMaterialBouncingComponent::SetImpactParamValue(UMaterialInstanceDynamic* DynamicMat)
{
	if (DynamicMat)
	{
		DynamicMat->SetVectorParameterValue(ImpactParamName, FVector(BounceNormal * BounceMultiplier * BouncingCurve->GetFloatValue(CurvePosition)));
	}
}
