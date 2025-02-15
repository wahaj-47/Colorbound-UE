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
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/ViewModels/ViewModel_Character.h"
#include "UI/ViewModels/ViewModel_Health.h"

AColorboundCharacterBase::AColorboundCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	CharacterViewModel = CreateDefaultSubobject<UViewModel_Character>(TEXT("CharacterViewModel"));
}

void AColorboundCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SpriteMaterialInstance = GetSprite()->CreateDynamicMaterialInstance(0, SpriteMaterial);

	AbilitySystemComponent->RegisterGameplayTagEvent(ColorboundGameplayTags::StatusTag_Hit, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitTagChanged);
	
	HealthChangedDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	MaxHealthChangedDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

	CharacterViewModel->GetCurrentHealthViewModel()->SetCurrentHealth(AttributeSet->GetHealth());
	CharacterViewModel->GetCurrentHealthViewModel()->SetMaxHealth(AttributeSet->GetMaxHealth());
}

UAbilitySystemComponent* AColorboundCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UColorboundAttributeSet* AColorboundCharacterBase::GetAttributeSet() const
{
	return AttributeSet;
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

void AColorboundCharacterBase::Die()
{
	Multicast_Die();
}

void AColorboundCharacterBase::Multicast_Die_Implementation()
{
	SetActorEnableCollision(false);
	StartDeathTimeline();
}

void AColorboundCharacterBase::HitTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AColorboundCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	CharacterViewModel->GetCurrentHealthViewModel()->SetCurrentHealth(Data.NewValue);
}

void AColorboundCharacterBase::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CharacterViewModel->GetCurrentHealthViewModel()->SetMaxHealth(Data.NewValue);
}
