// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MaxHealthModMagnitudeCalculation.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UMaxHealthModMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMaxHealthModMagnitudeCalculation();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition VigorDef;
	
};
