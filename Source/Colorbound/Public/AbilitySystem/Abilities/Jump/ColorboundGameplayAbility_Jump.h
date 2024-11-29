// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/ColorboundGameplayAbility.h"
#include "ColorboundGameplayAbility_Jump.generated.h"

struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundGameplayAbility_Jump : public UColorboundGameplayAbility
{
	GENERATED_BODY()

public:
	UColorboundGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	void CharacterJumpStop();
};
