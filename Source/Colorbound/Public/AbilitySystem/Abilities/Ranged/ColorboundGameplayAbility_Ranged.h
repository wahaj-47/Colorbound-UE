// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/ColorboundGameplayAbility.h"
#include "GameplayTagContainer.h" 
#include "ColorboundGameplayAbility_Ranged.generated.h"

class AColorboundProjectile;

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
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Colorbound|Ability|Animation")
	FGameplayTagContainer AttackAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	float Damage = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Projectile")
	TSubclassOf<AColorboundProjectile> ProjectileClass;

	// Number of projectiles to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Projectile")
	int32 Count = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Colorbound|Ability|Projectile")
	FVector2D SpawnOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Projectile")
	FVector2D Spread;

	UFUNCTION(BlueprintPure, Category="Colorbound|Ability")
	FTransform GetSpawnTransform() const;

};
