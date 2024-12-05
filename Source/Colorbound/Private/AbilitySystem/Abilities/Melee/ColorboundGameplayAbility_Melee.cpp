// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Melee/ColorboundGameplayAbility_Melee.h"
#include "Core/ColorboundCharacterBase.h"
#include "Characters/ColorboundEnemyCharacter.h"

UColorboundGameplayAbility_Melee::UColorboundGameplayAbility_Melee(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

