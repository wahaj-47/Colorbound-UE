// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorboundGameplayAbility.h"
#include "ColorboundGameplayAbility_Melee.generated.h"

class UPaperZDAnimSequence;

/**
 * 
 */
UCLASS()
class UColorboundGameplayAbility_Melee : public UColorboundGameplayAbility
{
	GENERATED_BODY()

	UFUNCTION()
	void OnAnimationEnd(bool bWasCompleted);

	UFUNCTION()
	void PerformHitScan(FGameplayEventData Payload);

	UPROPERTY()
	int8 AnimationIndex;

public:
	UColorboundGameplayAbility_Melee(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	
	UPROPERTY(EditDefaultsOnly, Category="Colorbound|Ability")
	TArray<UPaperZDAnimSequence*> AttackAnimations;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability")
	float Range;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability")
	float CapsuleRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability")
	float CapsuleHalfHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability")
	FGameplayTag EventTag_HitScan;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability|Damage")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability|Damage")
	FGameplayTag DamageTag;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Ability|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

};
