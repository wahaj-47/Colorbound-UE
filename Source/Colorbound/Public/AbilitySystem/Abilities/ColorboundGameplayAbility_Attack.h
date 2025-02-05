// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/ColorboundGameplayAbility.h"
#include "ColorboundGameplayAbility_Attack.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundGameplayAbility_Attack : public UColorboundGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Colorbound|Ability")
	TArray<FGameplayTagContainer> Animations;
	
};
