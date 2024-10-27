// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundCharacterBase.h"

#include "ColorboundAbilitySystemComponent.h"
#include "ColorboundAttributeSet.h"
#include "ColorboundAbilitySet.h"
#include "ColorboundGameplayTags.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDAnimationComponent.h"
#include "PaperZDAnimInstance.h"

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

void AColorboundCharacterBase::SetDirectionality(FVector2D Direction)
{
	ForwardVector = FVector(Direction.X, Direction.Y, 0);
	RightVector = ForwardVector.RotateAngleAxis(90, FVector::UpVector);
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

void AColorboundCharacterBase::HitReact()
{
	UPaperZDAnimSequence* Animation = GetAnimationSequence(FGameplayTagContainer(ColorboundGameplayTags::StatusTag_Hit));
	UPaperZDAnimInstance* AnimInst = GetAnimInstance();

	if (Animation && AnimInst)
	{
		AnimInst->PlayAnimationOverride(Animation);
	}

	K2_OnCharacterHit();
}
