// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AbilityQueueComponent.generated.h"

class UAbilitySystemComponent;
struct FGameplayEventData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COLORBOUND_API UAbilityQueueComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	const UGameplayAbility* QueuedAbility;

	void OpenAbilityQueue(FGameplayTag Tag, const FGameplayEventData* Payload);

	void CloseAbilityQueue(FGameplayTag Tag, const FGameplayEventData* Payload);

	UFUNCTION()
	void QueueAbility(const UGameplayAbility* Ability, const FGameplayTagContainer& Tags);

	UFUNCTION()
	void ActivateQueuedAbility(const FAbilityEndedData& AbilityEndedData);

public:	
	// Sets default values for this component's properties
	UAbilityQueueComponent();

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability System")
	void InitAbilityQueueComponent(UAbilitySystemComponent* ASC);

protected:
 	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability Queue|Tags")
	FGameplayTag EventTag_OpenQueue;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability Queue|Tags")
	FGameplayTag EventTag_CloseQueue;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

		
};
