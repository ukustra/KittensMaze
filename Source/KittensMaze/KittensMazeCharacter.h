// Copyright 2021 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "FootstepInterface.h"
#include "KittensMazeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAudioComponent;
class UWidgetComponent;
class UAbilitySystemComponent;
class UFootstepComponent;
class UGameplayAbility;
class UKittensMazeHUDWidget;
class UImpactDataObject;

UENUM(BlueprintType)
enum class EAbilityInput : uint8
{
	Jump,
	Attack,
	Bomb,
	Boost
};

USTRUCT(BlueprintType)
struct FInputAbilityData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	EAbilityInput InputType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	FORCEINLINE friend uint32 GetTypeHash(const FInputAbilityData& Data)
	{
		return GetTypeHash(Data.AbilityClass);
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterInitDelegate, class AKittensMazeCharacter*, Character, const FString&, ChracaterName);

UCLASS(config=Game)
class AKittensMazeCharacter : public ACharacter, public IAbilitySystemInterface, public IFootstepInterface
{
	GENERATED_UCLASS_BODY()

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* AudioListener;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* VoiceAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFootstepComponent* FootstepComponent;

	UPROPERTY()
	UMaterialInterface* DefaultFaceMat;

	UPROPERTY()
	TMap<FGameplayTag, USoundBase*> Voices;

	UPROPERTY()
	TSet<UStaticMeshComponent*> AdditionalMeshes;

	UPROPERTY()
	UImpactDataObject* ImpactData;

	FString CurrentCharacterPreset;

	void InitAbilitySystem();
	void OnAbilityActivated(UGameplayAbility* Ability);
	void OnAbilityEnded(const FAbilityEndedData& EndData);
	void InitAttributeDelegate(const FGameplayAttribute& Attribute);
	void OnAttributeChanged(const FOnAttributeChangeData& AttributeChangeData);

	void DealDamageInternal(const FHitResult& HitResult, bool bBombDamage, const FVector& DamageDirection, const FVector& SphereStart, const FVector& SphereEnd, float SphereRadius);

protected:
	UFUNCTION(BlueprintCallable, Category = "Kitty")
	void InitCharacter(const FString& CharacterName, bool bPreviewCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "AbilitySystem", meta = (DisplayName = "On Ability Activated"))
	void K2_OnAbilityActivated(const FGameplayTagContainer& AbilityTags);

	UFUNCTION(BlueprintImplementableEvent, Category = "AbilitySystem", meta = (DisplayName = "On Ability Ended"))
	void K2_OnAbilityEnded(const FGameplayTagContainer& AbilityTags, bool bWasCancelled);

	UFUNCTION(BlueprintImplementableEvent, Category = "AbilitySystem", meta = (DisplayName = "On Attribute Changed"))
	void K2_OnAttributeChanged(const FGameplayAttribute& Attribute, float NewValue, float OldValue);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kitty")
	TSubclassOf<UKittensMazeHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kitty")
	TSubclassOf<UImpactDataObject> ImpactDataClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kitty", meta = (Categories = "ObjectPooling"))
	FGameplayTag ImpactActorTag;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Kitty")
	UKittensMazeHUDWidget* HUDWidget;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Kitty")
	USoundBase* WeaponWhooshSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Kitty")
	float AttackMontageMultiplier;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Kitty")
	float BaseDamage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Kitty")
	FLinearColor WidgetColor;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Kitty")
	TArray<UWidgetComponent*> BoostWidgetComponents;

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TSet<FInputAbilityData> InputAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TSet<TSubclassOf<UGameplayAbility>> NonInputAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TSet<TSubclassOf<UAttributeSet>> AttributeSets;

	//~ Begin IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface interface

	//~ Begin IFootstepInterface interface
	virtual UFootstepComponent* GetFootstepComponent_Implementation() const override;
	//~ End IFootstepInterface interface

	//~ Begin APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
public:
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn interface

	UPROPERTY(BlueprintAssignable, Category = "Kitty")
	FCharacterInitDelegate OnCharacterInited;

	UFUNCTION(BlueprintImplementableEvent, Category = "Game", meta = (DisplayName = "On All Players Restarted"))
	void K2_OnAllPlayersRestarted();

	UFUNCTION(BlueprintCallable, Category = "Kitty")
	bool PlayVoiceOver(UPARAM(meta = (Categories = "VO")) const FGameplayTag VoiceTag);

	UFUNCTION(BlueprintCallable, Category = "Kitty")
	void SetFaceMaterial(UMaterialInterface* FaceMat);

	UFUNCTION(BlueprintCallable, Category = "Kitty")
	void ResetFaceMaterial();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void DealDamage(bool bBombDamage, const FVector& DamageDirection, const FVector& SphereStart, const FVector& SphereEnd, float SphereRadius = 10.f);

	//~ Begin AActor interface
	virtual void PostInitializeComponents() override;
	//~ End AActor interface

	//~ Begin ACharacter interface
	virtual void SetBase(UPrimitiveComponent* NewBase, FName BoneName, bool bNotifyActor) override;
	//~ End ACharacter interface

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
