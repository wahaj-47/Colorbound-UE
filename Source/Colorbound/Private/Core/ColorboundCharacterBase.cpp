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

void AColorboundCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AColorboundCharacterBase, ForwardVector, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AColorboundCharacterBase, RightVector, COND_None, REPNOTIFY_Always);
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
	Server_SetDirectionality(Direction);
	UpdateDirectionality(Direction);
}

void AColorboundCharacterBase::Server_SetDirectionality_Implementation(FVector2D Direction)
{
	UpdateDirectionality(Direction);
}

void AColorboundCharacterBase::UpdateDirectionality(FVector2D Direction)
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
