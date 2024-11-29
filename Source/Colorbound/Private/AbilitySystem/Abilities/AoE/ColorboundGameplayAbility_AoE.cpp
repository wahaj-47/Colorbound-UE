// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AoE/ColorboundGameplayAbility_AoE.h"

UColorboundGameplayAbility_AoE::UColorboundGameplayAbility_AoE(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}
