// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ColorboundGameplayAbility.generated.h"

class AColorboundCharacterBase;
class UPaperZDAnimInstance;

/**
 * Defines how ability is meant to activate
 */
UENUM(BlueprintType)
enum class EColorboundAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive
};

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	EColorboundAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability Activation")
	EColorboundAbilityActivationPolicy ActivationPolicy;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	AColorboundCharacterBase* GetColorboundCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	UPaperZDAnimInstance* GetAnimInstanceFromActorInfo() const;
	
};
