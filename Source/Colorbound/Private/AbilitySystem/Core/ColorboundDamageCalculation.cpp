// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/ColorboundDamageCalculation.h"
#include "AbilitySystem/Core/ColorboundAttributeSet.h"
#include "Core/ColorboundCharacterBase.h"
#include "AbilitySystem/Core/ColorboundAbilityTypes.h"
#include "Interfaces/CharacterStatsInterface.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition DamageDef;

	FGameplayEffectAttributeCaptureDefinition DamageResistanceDef;

	FGameplayEffectAttributeCaptureDefinition BlockChanceDef;
	FGameplayEffectAttributeCaptureDefinition CriticalHitChanceDef;
	FGameplayEffectAttributeCaptureDefinition CriticalHitDamageDef;
	FGameplayEffectAttributeCaptureDefinition CriticalHitResistanceDef;
	FGameplayEffectAttributeCaptureDefinition ArmorDef;
	FGameplayEffectAttributeCaptureDefinition ArmorPenetrationDef;

	FDamageStatics()
	{
		DamageDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		
		DamageResistanceDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetDamageResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);

		BlockChanceDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetBlockChanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		ArmorDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetArmorAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		ArmorPenetrationDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetArmorPenetrationAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalHitChanceDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetCriticalHitChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalHitDamageDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetCriticalHitDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalHitResistanceDef = FGameplayEffectAttributeCaptureDefinition(UColorboundAttributeSet::GetCriticalHitResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
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

	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
}

void UColorboundDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	
	AColorboundCharacterBase* SourceActor = SourceASC ? Cast<AColorboundCharacterBase>(SourceASC->GetAvatarActor()) : nullptr;
	AColorboundCharacterBase* TargetActor = TargetASC ? Cast<AColorboundCharacterBase>(TargetASC->GetAvatarActor()) : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FColorboundGameplayEffectContext* Context = static_cast<FColorboundGameplayEffectContext*>(Spec.GetContext().Get());

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
	Damage = FMath::Max<float>(Damage, 0.f);

	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	Context->SetBlockedHit(bBlocked);
	
	// If blocked, no damage
	if (bBlocked)
	{
		Damage = 0.f;
	}

	float DamageResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageResistanceDef, EvaluationParameters, DamageResistance);
	DamageResistance = FMath::Clamp(DamageResistance, 0.f, 100.f);
	
	// DamageResistance ignores a percentage of incoming Damage
	Damage *= (100.f - DamageResistance) / 100.f;

	if (Damage > 0.f)
	{
		float TargetArmor = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
		TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

		float SourceArmorPenetration = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
		SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

		const FRealCurve* ArmorPenetrationCurve = DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
		const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(ICharacterStatsInterface::Execute_GetCharacterLevel(SourceActor));

		// ArmorPenetration ignores a percentage of the Target's Armor
		const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

		const FRealCurve* EffectiveArmorCurve = DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
		const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(ICharacterStatsInterface::Execute_GetCharacterLevel(TargetActor));

		// Armor ignores a percentage of the incoming damage
		Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

		float SourceCriticalHitChance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
		SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

		float SourceCriticalHitDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
		SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

		float TargetCriticalHitResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
		TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

		const FRealCurve* CriticalHitResistanceCurve = DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
		const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(ICharacterStatsInterface::Execute_GetCharacterLevel(TargetActor));

		// CriticalHitResistance reduces CriticalHitChance by a percentage
		const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
		const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
		Context->SetCriticalHit(bCriticalHit);

		// If critical hit, double the damage and add CriticalHitDamage bonus
		if (bCriticalHit)
		{
			Damage = 2.f * Damage + SourceCriticalHitDamage;
		}

	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UColorboundAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
}
