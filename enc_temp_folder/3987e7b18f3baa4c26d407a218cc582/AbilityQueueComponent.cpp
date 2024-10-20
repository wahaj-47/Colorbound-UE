// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ActorComponents/AbilityQueueComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

//// Sets default values for this component's properties
UAbilityQueueComponent::UAbilityQueueComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityQueueComponent::InitAbilityQueueComponent(UAbilitySystemComponent* ASC)
{
	AbilitySystemComponent = ASC;
	check(AbilitySystemComponent);

	AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &ThisClass::ActivateQueuedAbility);
	AbilitySystemComponent->AddGameplayEventTagContainerDelegate
	(
		FGameplayTagContainer(EventTag_OpenQueue),
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OpenAbilityQueue)
	);
	AbilitySystemComponent->AddGameplayEventTagContainerDelegate(
		FGameplayTagContainer(EventTag_CloseQueue),
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::CloseAbilityQueue)
	);
}

void UAbilityQueueComponent::OpenAbilityQueue(FGameplayTag Tag, const FGameplayEventData* Payload)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, TEXT("Opening ability queue"));
	//AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &ThisClass::QueueAbility);
}

void UAbilityQueueComponent::CloseAbilityQueue(FGameplayTag Tag, const FGameplayEventData* Payload)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, TEXT("Closing ability queue"));
	//AbilitySystemComponent->AbilityFailedCallbacks.RemoveAll(this);
}

void UAbilityQueueComponent::QueueAbility(const UGameplayAbility* Ability, const FGameplayTagContainer& Tags)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, FString::Printf(TEXT("Ability failed: %s"), *Ability->GetName()));
	QueuedAbilityTags = Tags;
}

void UAbilityQueueComponent::ActivateQueuedAbility(const FAbilityEndedData& AbilityEndedData)
{
	if (!QueuedAbilityTags.IsEmpty())
	{
		//AbilitySystemComponent->TryActivateAbilitiesByTag(QueuedAbilityTags);
		QueuedAbilityTags.Reset();
	}
}




