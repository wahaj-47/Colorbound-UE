// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundCharacterBase.h"

#include "PaperFlipbookComponent.h"
#include "ColorboundAbilitySystemComponent.h"
#include "ColorboundAttributeSet.h"
#include "ColorboundAbilitySet.h"

void AColorboundCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SpriteMaterialInstance = GetSprite()->CreateDynamicMaterialInstance(0, SpriteMaterial);
}

UAbilitySystemComponent* AColorboundCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UColorboundAttributeSet* AColorboundCharacterBase::GetAttributeSet() const
{
	return AttributeSet;
}

int32 AColorboundCharacterBase::GetCharacterLevel() const
{
	if (AttributeSet != nullptr)
	{
		return static_cast<int32>(AttributeSet->GetCharacterLevel());
	}

	return 1;
}

void AColorboundCharacterBase::InitializeAbilitySet()
{
	if (AbilitySet)
	{
		UColorboundAbilitySystemComponent* ASC = CastChecked<UColorboundAbilitySystemComponent>(GetAbilitySystemComponent());
		AbilitySet->GiveToAbilitySystem(ASC, nullptr, this);
	}
}

FVector AColorboundCharacterBase::GetForwardVector() const
{
	return ForwardVector;
}

FVector AColorboundCharacterBase::GetRightVector() const
{
	return RightVector;
}
