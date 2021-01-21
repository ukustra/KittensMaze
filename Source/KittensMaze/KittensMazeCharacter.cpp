// Copyright 2021 Urszula Kustra. All Rights Reserved.

#include "KittensMazeCharacter.h"
#include "KittensMazeGameInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/GameModeBase.h"
#include "AbilitySystemComponent.h"
#include "FootstepComponent.h"
#include "KittensMazeSettings.h"
#include "VoxelWorld.h"
#include "VoxelRender/VoxelProceduralMeshComponent.h"
#include "AbilitySystem/InventoryAttributeSet.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/KittensMazeHUDWidget.h"
#include "ObjectPooling/ObjectPoolingSubsystem.h"
#include "ObjectPooling/ImpactDataObject.h"

AKittensMazeCharacter::AKittensMazeCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AudioListener = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("AudioListener"));
	AudioListener->SetupAttachment(FollowCamera);

	WeaponMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("WeaponMesh"));
	WeaponMesh->SetCanEverAffectNavigation(false);
	WeaponMesh->SetVisibility(false);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("hand_socket_r"));

	VoiceAudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("VoiceAudioComponent"));
	VoiceAudioComponent->SetAutoActivate(false);
	VoiceAudioComponent->bAllowSpatialization = false;
	VoiceAudioComponent->SetupAttachment(GetRootComponent());

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->AbilityActivatedCallbacks.AddUObject(this, &AKittensMazeCharacter::OnAbilityActivated);
	AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &AKittensMazeCharacter::OnAbilityEnded);

	FootstepComponent = ObjectInitializer.CreateDefaultSubobject<UFootstepComponent>(this, TEXT("FootstepComponent"));
}

void AKittensMazeCharacter::InitCharacter(const FString& CharacterName, bool bPreviewCharacter)
{
	if (!GetWorld() || CurrentCharacterPreset == CharacterName)
	{
		return;
	}

	bool bLoadedPreset = false;
	if (GetMesh() && GetCharacterMovement() && WeaponMesh)
	{
		if (const UKittensMazeGameInstance* GameInstance = Cast<UKittensMazeGameInstance>(GetGameInstance()))
		{
			const FCharacterInfo& CharacterPreset = GameInstance->GetCharacterPreset(CharacterName);
			if (CharacterPreset.IsValid())
			{
				GetMesh()->SetMaterial(0, CharacterPreset.Material);
				GetMesh()->SetMaterial(1, CharacterPreset.Eyes);
				DefaultFaceMat = CharacterPreset.Eyes;

				WeaponMesh->SetStaticMesh(CharacterPreset.WeaponMesh);
				if (bPreviewCharacter && !WeaponMesh->IsVisible())
				{
					WeaponMesh->SetVisibility(true);
				}

				for (auto& AdditionalMesh : AdditionalMeshes)
				{
					if (AdditionalMesh)
					{
						AdditionalMesh->DestroyComponent();
					}
				}
				AdditionalMeshes.Empty();
				Voices.Empty();

				int32 Index = 0;
				for (const auto& It : CharacterPreset.AdditionalMeshes)
				{
					if (UStaticMesh* StaticMesh = It.Value)
					{
						if (GetMesh()->DoesSocketExist(It.Key))
						{
							UStaticMeshComponent* AdditionalMeshComponent = NewObject<UStaticMeshComponent>(this, *(StaticMesh->GetName() + FString::FromInt(Index)));
							AdditionalMeshComponent->SetStaticMesh(StaticMesh);
							AdditionalMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
							AdditionalMeshComponent->SetCanEverAffectNavigation(false);
							AdditionalMeshComponent->SetRenderCustomDepth(true);
							AdditionalMeshComponent->SetupAttachment(GetMesh(), It.Key);

							AdditionalMeshComponent->RegisterComponent();

							AdditionalMeshes.Add(AdditionalMeshComponent);
						}
					}

					++Index;
				}
				
				WeaponWhooshSound = CharacterPreset.WeaponWhooshSound;
				AttackMontageMultiplier = CharacterPreset.AttackMontageMultiplier;
				BaseDamage = CharacterPreset.Damage;

				GetCharacterMovement()->JumpZVelocity = CharacterPreset.JumpPower;
				GetCharacterMovement()->GravityScale = CharacterPreset.GravityScale;

				WidgetColor = CharacterPreset.WidgetColor;

				CurrentCharacterPreset = CharacterName;

				bLoadedPreset = true;

				if (!bPreviewCharacter)
				{
					Voices = CharacterPreset.Voices;
					
					if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
					{
						GameMode->ChangeName(GetController(), CurrentCharacterPreset, true);
					}
				}

				OnCharacterInited.Broadcast(this, CharacterName);
			}
		}
	}
	
	if (!bLoadedPreset && GetMesh())
	{
		if (UMaterialInterface* FaceMat = GetMesh()->GetMaterial(1))
		{
			DefaultFaceMat = FaceMat;
		}
	}
}

UAbilitySystemComponent* AKittensMazeCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFootstepComponent* AKittensMazeCharacter::GetFootstepComponent_Implementation() const
{
	return FootstepComponent;
}

void AKittensMazeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	AbilitySystemComponent->BindToInputComponent(PlayerInputComponent);

	for (const auto& InputAbilityData : InputAbilities)
	{
		if (InputAbilityData.AbilityClass)
		{
			const int32 AbilityID = (int32)InputAbilityData.InputType;
			const FGameplayAbilitySpec Spec = FGameplayAbilitySpec(InputAbilityData.AbilityClass, 1, AbilityID);
			AbilitySystemComponent->GiveAbility(Spec);

			const FGameplayAbilityInputBinds InputBinds = FGameplayAbilityInputBinds(TEXT("AbilityConfirm"), TEXT("AbilityCancel"), TEXT("EAbilityInput"), AbilityID, AbilityID);
			AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, InputBinds);
		}
	}
}

void AKittensMazeCharacter::PossessedBy(AController* NewController)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		if (!HUDWidget && HUDWidgetClass)
		{
			HUDWidget = CreateWidget<UKittensMazeHUDWidget>(PlayerController, HUDWidgetClass);
			HUDWidget->Init();
			HUDWidget->AddToPlayerScreen();
			HUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}

		if (!ImpactData && ImpactDataClass)
		{
			ImpactData = NewObject<UImpactDataObject>(this, ImpactDataClass);
		}
	}

	Super::PossessedBy(NewController);
}

bool AKittensMazeCharacter::PlayVoiceOver(const FGameplayTag VoiceTag)
{
	if (VoiceAudioComponent)
	{
		if (USoundBase* VoiceSound = Voices.FindRef(VoiceTag))
		{
			VoiceAudioComponent->Stop();
			VoiceAudioComponent->SetSound(VoiceSound);
			VoiceAudioComponent->Play();

			return true;
		}
	}

	return false;
}

void AKittensMazeCharacter::SetFaceMaterial(UMaterialInterface* FaceMat)
{
	if (FaceMat && GetMesh())
	{
		GetMesh()->SetMaterial(1, FaceMat);
	}
}

void AKittensMazeCharacter::ResetFaceMaterial()
{
	if (DefaultFaceMat && GetMesh())
	{
		GetMesh()->SetMaterial(1, DefaultFaceMat);
	}
}

void AKittensMazeCharacter::DealDamage(bool bBombDamage, const FVector& DamageDirection, const FVector& SphereStart,
                                       const FVector& SphereEnd, float SphereRadius)
{
	if (!GetWorld())
	{
		return;
	}

	const ECollisionChannel CollisionChannel = ECC_Visibility;
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
	const FCollisionQueryParams QueryParams = Invoke([]()->FCollisionQueryParams const
	{
		FCollisionQueryParams Params;
		Params.bReturnPhysicalMaterial = true;
		Params.bTraceComplex = false;

		return Params;
	});

	if (bBombDamage)
	{
		TArray<FHitResult> HitResults;
		const bool bSweep = GetWorld()->SweepMultiByChannel(HitResults, SphereStart, SphereEnd, FQuat::Identity, CollisionChannel, SphereShape, QueryParams);
		if (bSweep)
		{
			for (const FHitResult& HitResult : HitResults)
			{
				DealDamageInternal(HitResult, bBombDamage, DamageDirection, SphereStart, SphereEnd, SphereRadius);
			}
		}
	}
	else
	{
		FHitResult HitResult;
		const bool bSweep = GetWorld()->SweepSingleByChannel(HitResult, SphereStart, SphereEnd, FQuat::Identity, CollisionChannel, SphereShape, QueryParams);
		if (bSweep)
		{
			DealDamageInternal(HitResult, bBombDamage, DamageDirection, SphereStart, SphereEnd, SphereRadius);
		}
	}
}

void AKittensMazeCharacter::PostInitializeComponents()
{
	InitAbilitySystem();

	Super::PostInitializeComponents();
}

inline UPrimitiveComponent* GetVoxelWorldBase(AVoxelWorld* World)
{
	auto* Root = Cast<UPrimitiveComponent>(World->GetRootComponent());
	ensure(Root != nullptr);
	return Root;
}

void AKittensMazeCharacter::SetBase(UPrimitiveComponent* NewBase, FName BoneName, bool bNotifyActor)
{
	if (auto* Component = Cast<UVoxelProceduralMeshComponent>(NewBase))
	{
		if (auto* World = Cast<AVoxelWorld>(Component->GetOwner()))
		{
			Super::SetBase(GetVoxelWorldBase(World), BoneName, bNotifyActor);
		}
		else
		{
			LOG_VOXEL(Warning, TEXT("VoxelCharacter: Invalid Voxel Proc Mesh Component!"));
		}
	}
	else if (auto* InstancedComponent = Cast<UInstancedStaticMeshComponent>(NewBase))
	{
		if (auto* World = Cast<AVoxelWorld>(InstancedComponent->GetOwner()))
		{
			Super::SetBase(GetVoxelWorldBase(World), BoneName, bNotifyActor);
		}
		else
		{
			Super::SetBase(NewBase, BoneName, bNotifyActor);
		}
	}
	else
	{
		Super::SetBase(NewBase, BoneName, bNotifyActor);
	}
}

void AKittensMazeCharacter::InitAbilitySystem()
{
	for (const auto& Ability : NonInputAbilities)
	{
		if (Ability)
		{
			AbilitySystemComponent->GiveAbility(Ability.GetDefaultObject());
		}
	}

	for (const auto& Set : AttributeSets)
	{
		if (Set)
		{
			AbilitySystemComponent->InitStats(Set, nullptr);
		}
	}

	InitAttributeDelegate(UInventoryAttributeSet::GetCoinsAmountAttribute());
	InitAttributeDelegate(UInventoryAttributeSet::GetBombsAmountAttribute());
	InitAttributeDelegate(UInventoryAttributeSet::GetBoostAmountAttribute());
}

void AKittensMazeCharacter::OnAbilityActivated(UGameplayAbility* Ability)
{
	if (Ability)
	{
		K2_OnAbilityActivated(Ability->AbilityTags);
	}
}

void AKittensMazeCharacter::OnAbilityEnded(const FAbilityEndedData& EndData)
{
	if (const UGameplayAbility* Ability = EndData.AbilityThatEnded)
	{
		K2_OnAbilityEnded(Ability->AbilityTags, EndData.bWasCancelled);
	}
}

void AKittensMazeCharacter::InitAttributeDelegate(const FGameplayAttribute& Attribute)
{
	if (Attribute.IsValid())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &AKittensMazeCharacter::OnAttributeChanged);
	}
}

void AKittensMazeCharacter::OnAttributeChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	K2_OnAttributeChanged(AttributeChangeData.Attribute, AttributeChangeData.NewValue, AttributeChangeData.OldValue);
}

void AKittensMazeCharacter::DealDamageInternal(const FHitResult& HitResult, bool bBombDamage,
	const FVector& DamageDirection, const FVector& SphereStart, const FVector& SphereEnd, float SphereRadius)
{
	if (!FollowCamera)
	{
		return;
	}

	if (const UKittensMazeSettings* KittensMazeSettings = UKittensMazeSettings::Get())
	{
		const float DamageToApply = bBombDamage ? KittensMazeSettings->GetBombDamageMultiplier() * BaseDamage : BaseDamage;
		const float AppliedDamage = UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), DamageToApply, DamageDirection, HitResult, GetController(), this, nullptr);

		const bool bHitComponentVisible = HitResult.Component.IsValid() ? (HitResult.GetComponent()->IsVisible() || !HitResult.GetComponent()->bHiddenInGame) : false;

		if (ImpactData && ImpactActorTag.IsValid() && AppliedDamage > 0.f && bHitComponentVisible)
		{
			if (UObjectPoolingSubsystem* ObjectPoolingSubsystem = GetWorld()->GetSubsystem<UObjectPoolingSubsystem>())
			{
				const FVector& ImpactPoint = HitResult.ImpactPoint;
				const ECollisionChannel ComponentCollision = HitResult.GetComponent()->GetCollisionObjectType();
				const bool bPlayWhoosh = HitResult.GetActor()->CanBeDamaged() && (ComponentCollision == ECC_Destructible || ECC_PhysicsBody);
				ImpactData->bPlayWhoosh = bPlayWhoosh;
				if (bPlayWhoosh)
				{
					const FVector WhooshDirection = (FollowCamera->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
					const FQuat WhooshQuat = FRotationMatrix::MakeFromX(WhooshDirection).ToQuat();
					const FQuat WhooshRotationOffset = FRotator(0.f, FMath::RandRange(-45.f, 45.f), 0.f).Quaternion();
					const FQuat CombinedQuats = WhooshQuat * WhooshRotationOffset;
					const FVector WhooshScale = FVector(FMath::RandRange(0.f, 1.f));

					ImpactData->WhooshTransform = FTransform(CombinedQuats, ImpactPoint, WhooshScale);
					ImpactData->bHitDestructible = HitResult.GetComponent()->GetCollisionObjectType() == ECC_Destructible;
				}

				const FQuat ImpactQuat = FRotationMatrix::MakeFromX(ImpactPoint).ToQuat();
				const FTransform ImpactTransform = FTransform(ImpactQuat, ImpactPoint);

				ObjectPoolingSubsystem->ActivateActorFromPool(ImpactActorTag, ImpactTransform, ImpactData);
			}
		}
	}
}
