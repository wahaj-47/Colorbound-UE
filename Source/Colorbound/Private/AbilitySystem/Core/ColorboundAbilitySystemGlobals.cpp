// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/ColorboundAbilitySystemGlobals.h"
#include "AbilitySystem/Abilities/ColorboundAbilityTypes.h"

FGameplayAbilityActorInfo* UColorboundAbilitySystemGlobals::AllocAbilityActorInfo() const
{
    return new FColorboundGameplayAbilityActorInfo();
}
