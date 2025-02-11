// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ColorboundEnemyCharacter.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "AbilitySystem/Core/ColorboundAttributeSet.h"
#include "AI/ColorboundAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AColorboundEnemyCharacter::AColorboundEnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UColorboundAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UColorboundAttributeSet>(TEXT("AttributeSet"));
}

int32 AColorboundEnemyCharacter::GetCharacterLevel_Implementation() const
{
	return CharacterLevel;
}

int32 AColorboundEnemyCharacter::GetCharacterXP_Implementation() const
{
	return static_cast<int32>(CharacterXP.GetValueAtLevel(CharacterLevel));
}

void AColorboundEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAbilitySet();
}

void AColorboundEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}

	ColorboundAIController = Cast<AColorboundAIController>(NewController);
	ColorboundAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	ColorboundAIController->RunBehaviorTree(BehaviorTree);
	ColorboundAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);

}

void AColorboundEnemyCharacter::HitTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::HitTagChanged(CallbackTag, NewCount);
	if (ColorboundAIController && ColorboundAIController->GetBlackboardComponent())
	{
		ColorboundAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}
