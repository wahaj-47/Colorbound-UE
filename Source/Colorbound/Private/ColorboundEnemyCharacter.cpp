// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundEnemyCharacter.h"
#include "ColorboundAbilitySystemComponent.h"
#include "ColorboundAttributeSet.h"

AColorboundEnemyCharacter::AColorboundEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UColorboundAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UColorboundAttributeSet>(TEXT("AttributeSet"));
}

void AColorboundEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAbilitySet();
}
