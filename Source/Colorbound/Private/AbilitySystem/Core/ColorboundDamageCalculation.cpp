// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/ColorboundDamageCalculation.h"
#include "AbilitySystem/Core/ColorboundAttributeSet.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition DamageDef;
	FGameplayEffectAttributeCaptureDefinition DamageResistanceDef;

	FDamageStatics()
	{
		DamageDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		DamageResistanceDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetDamageResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const FDamageStatics DamageStatics()
{
	static FDamageStatics DamageStatics;
	return DamageStatics;
}

UColorboundDamageCalculation::UColorboundDamageCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageResistanceDef);
}

void UColorboundDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float DamageResistance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageResistanceDef, EvaluationParameters, DamageResistance);

	float Damage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
	Damage *= (1 - DamageResistance);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UColorboundAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
}
