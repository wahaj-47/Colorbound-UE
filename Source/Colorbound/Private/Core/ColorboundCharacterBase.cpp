// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ColorboundCharacterBase.h"

#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "AbilitySystem/Core/ColorboundAttributeSet.h"
#include "AbilitySystem/Core/ColorboundAbilitySet.h"
#include "ColorboundGameplayTags.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDAnimationComponent.h"
#include "PaperZDAnimInstance.h"
#include "Net/UnrealNetwork.h"

AColorboundCharacterBase::AColorboundCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

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

UPaperZDAnimSequence* AColorboundCharacterBase::GetAnimationSequence(const FGameplayTagContainer& Rules) const
{
	for (const FColorboundAbilityAnimation& Rule : AnimationRules)
	{
		if (!Rule.Animation.IsNull() && Rules.HasAll(Rule.RequiredTags))
		{
			return Rule.Animation.LoadSynchronous();
		}
	}

	return nullptr;
}
