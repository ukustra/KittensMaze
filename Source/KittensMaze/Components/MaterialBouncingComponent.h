// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MaterialBouncingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBounceStartedDelegate, float, Radius);

/**
 * @see https://www.youtube.com/watch?v=SnJarsAsWwo
 */
UCLASS( ClassGroup=(Rendering), meta=(BlueprintSpawnableComponent) )
class KITTENSMAZE_API UMaterialBouncingComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bouncing")
	UCurveFloat* BouncingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bouncing")
	FName PositionParamName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bouncing")
	FName ImpactParamName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bouncing")
	FName RadiusParamName;

	/** Scaled with damage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bouncing", meta = (ClampMin = 0.f))
	float MinRadius;

	/** Scaled with damage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bouncing", meta = (ClampMin = 0.f))
	float MaxRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bouncing", meta = (ClampMin = 0.f))
	float BounceMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bouncing", meta = (ClampMin = 0.f))
	float PlayRate;

public:
	UPROPERTY(BlueprintAssignable, Category = "Rendering")
	FBounceStartedDelegate OnBounceStarted;

	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void InitDynamicMaterial(UMaterialInstanceDynamic* DynamicMaterial);

	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void InitDynamicMaterials(const TSet<UMaterialInstanceDynamic*>& DynamicMaterials);

	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void InitDynamicMaterialsFromMesh(UMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void Bounce(float Damage, const FVector& ImpactLocation, const FVector& ImpactNormal);

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent interface

	bool IsSetValid() const;

private:
	UPROPERTY()
	TSet<UMaterialInstanceDynamic*> DynamicMats;

	float CurveMin;
	float CurveMax;
	float CurvePosition;
	FVector BounceNormal;

	void SetImpactParamValue(UMaterialInstanceDynamic* DynamicMat);
};
