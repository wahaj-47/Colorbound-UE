// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ColorboundAbilitySystemLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/*
	* Effect Context Getters
	*/

	UFUNCTION(BlueprintPure, Category = "Colorbound|EffectContext", meta = (DisplayName = "IsBlockedHit"))
	static bool EffectContextIsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "Colorbound|EffectContext", meta = (DisplayName = "IsCriticalHit"))
	static bool EffectContextIsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
};
