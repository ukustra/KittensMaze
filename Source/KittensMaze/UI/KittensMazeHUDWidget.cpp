// Copyright 2021 Urszula Kustra. All Rights Reserved.


#include "KittensMazeHUDWidget.h"
#include "KittensMazeAttributeWidget.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "../AbilitySystem/InventoryAttributeSet.h"
#include "KittensMaze/KittensMazeCharacter.h"

UKittensMazeHUDWidget::UKittensMazeHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UKittensMazeHUDWidget::SetAttributeWidgetValue(const FGameplayAttribute& Attribute, float NewValue, bool bPlayAnim)
{
	if (UKittensMazeAttributeWidget* AttributeWidget = AttributeWidgets.FindRef(Attribute))
	{
		AttributeWidget->SetAttributeText(FText::AsNumber(NewValue));

		if (bPlayAnim)
		{
			AttributeWidget->PlayAnim();
		}
	}
}

void UKittensMazeHUDWidget::Init()
{
	if (AttributeWidgets.Num() == 0)
	{
		InitAttributeWidget(UInventoryAttributeSet::GetCoinsAmountAttribute(), GetCoinsWidget());
		InitAttributeWidget(UInventoryAttributeSet::GetBombsAmountAttribute(), GetBombsWidget());
		InitAttributeWidget(UInventoryAttributeSet::GetBoostAmountAttribute(), GetBoostWidget());
	}
}

void UKittensMazeHUDWidget::InitAttributeWidget(const FGameplayAttribute& Attribute, UKittensMazeAttributeWidget* AttributeWidget)
{
	if (!AttributeWidget)
	{
		return;
	}

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningPawn))
		{
			AttributeWidgets.Add(Attribute, AttributeWidget);
			AttributeWidget->SetAttributeText( FText::AsNumber(ASC->GetNumericAttribute(Attribute)) );
		}
	}
}
