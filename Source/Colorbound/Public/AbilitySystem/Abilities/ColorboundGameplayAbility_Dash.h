// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorboundGameplayAbility.h"
#include "ColorboundGameplayAbility_Dash.generated.h"

struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundGameplayAbility_Dash : public UColorboundGameplayAbility
{
	GENERATED_BODY()

public:
	UColorboundGameplayAbility_Dash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	void CharacterDashStart();

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	void CharacterDashStop();

	UFUNCTION(BlueprintPure, Category = "Colorbound|Ability|Utils")
	FVector GetFurthestLocationAlongPath() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability")
	float DashDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability")
	float MaxDashDistance;
};
