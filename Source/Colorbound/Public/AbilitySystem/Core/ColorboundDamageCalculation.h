// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ColorboundDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UColorboundDamageCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;
};
