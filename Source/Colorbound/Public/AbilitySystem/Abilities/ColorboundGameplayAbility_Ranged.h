// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorboundGameplayAbility.h"
#include "GameplayTagContainer.h" 
#include "ColorboundGameplayAbility_Ranged.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundGameplayAbility_Ranged : public UColorboundGameplayAbility
{
	GENERATED_BODY()
	
public:
	UColorboundGameplayAbility_Ranged(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	UPROPERTY(BlueprintReadWrite, Category = "Colorbound|Abiliy")
	int32 AnimationIndex;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Colorbound|Ability")
	TArray<FGameplayTagContainer> AttackAnimations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

};
