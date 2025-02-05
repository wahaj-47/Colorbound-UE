// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/ColorboundGameplayAbility.h"
#include "ColorboundGameplayAbility_Aura.generated.h"

class USphereComponent;
class AColorboundPlayerCharacter;

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundGameplayAbility_Aura : public UColorboundGameplayAbility
{
	GENERATED_BODY()

public:
	UColorboundGameplayAbility_Aura(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Colorbound|Ability|Aura")
	TObjectPtr<USphereComponent> AuraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colorbound|Ability|Aura")
	float AuraRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability|Effect")
	TSubclassOf<UGameplayEffect> AuraEffectClass;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability|Component")
	void AddAuraComponent();
};
