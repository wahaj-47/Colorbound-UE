// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/ColorboundAbilitySystemLibrary.h"
#include "AbilitySystem/Core/ColorboundAbilityTypes.h"

bool UColorboundAbilitySystemLibrary::EffectContextIsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FColorboundGameplayEffectContext* ColorboundEffectContext = static_cast<const FColorboundGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ColorboundEffectContext->GetBlockedHit();
	}
	return false;
}

bool UColorboundAbilitySystemLibrary::EffectContextIsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FColorboundGameplayEffectContext* ColorboundEffectContext = static_cast<const FColorboundGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ColorboundEffectContext->GetCriticalHit();
	}
	return false;
}
