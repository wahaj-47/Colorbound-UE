// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/ColorboundGameplayAbility.h"
#include "ColorboundGameplayAbility_AoE.generated.h"

class AColorboundAreaOfEffectActor;

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundGameplayAbility_AoE : public UColorboundGameplayAbility
{
	GENERATED_BODY()

public:
	UColorboundGameplayAbility_AoE(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Colorbound|Ability|Animation")
	FGameplayTagContainer AttackAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	TSubclassOf<AColorboundAreaOfEffectActor> EffectDeliveryActorClass;
	
};
