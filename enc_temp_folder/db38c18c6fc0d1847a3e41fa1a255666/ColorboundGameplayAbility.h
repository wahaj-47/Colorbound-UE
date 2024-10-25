// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ColorboundGameplayAbility.generated.h"

class AColorboundCharacterBase;
class AColorboundAbilitySystemComponent;
class UPaperZDAnimInstance;

/**
 * Defines how ability is meant to activate
 */
UENUM(BlueprintType)
enum class EColorboundAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnGranted
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

	void TryActivateAbilityOnGranted(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

protected:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability Activation")
	EColorboundAbilityActivationPolicy ActivationPolicy;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	AColorboundCharacterBase* GetColorboundCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	UColorboundAbilitySystemComponent* GetColorboundAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	UPaperZDAnimInstance* GetAnimInstanceFromActorInfo() const;
	
};
