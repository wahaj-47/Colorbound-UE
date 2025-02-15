// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/MaxHealthModMagnitudeCalculation.h"
#include "AbilitySystem/Core/ColorboundAttributeSet.h"
#include "Core/ColorboundCharacterBase.h"
#include "Interfaces/CharacterStatsInterface.h"

UMaxHealthModMagnitudeCalculation::UMaxHealthModMagnitudeCalculation()
{
	VigorDef.AttributeToCapture = UColorboundAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMaxHealthModMagnitudeCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	int32 CharacterLevel = 1;
	UObject* SourceObject = Spec.GetContext().GetSourceObject();
	if (SourceObject->Implements<UCharacterStatsInterface>())
	{
		CharacterLevel = ICharacterStatsInterface::Execute_GetCharacterLevel(SourceObject);
	}

	return 80.f + 2.5f * Vigor + 10.f * CharacterLevel;
}
