// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Ranged/ColorboundGameplayAbility_Ranged.h"
#include "Core/ColorboundCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

UColorboundGameplayAbility_Ranged::UColorboundGameplayAbility_Ranged(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

FTransform UColorboundGameplayAbility_Ranged::GetSpawnTransform() const
{
	AColorboundCharacterBase* Character = GetColorboundCharacterFromActorInfo();
	FVector Forward = Character->GetForwardVector();
	FVector Right = Character->GetRightVector();
	FVector SpawnLocation = Character->GetActorLocation() + (Forward * SpawnOffset.Y) + (Right * UKismetMathLibrary::RandomFloatInRange(-SpawnOffset.X,SpawnOffset.X));
	FRotator SpawnRotation = UKismetMathLibrary::MakeRotFromX(Forward);
	SpawnRotation.Yaw += UKismetMathLibrary::RandomFloatInRange(-Spread.X, Spread.X);
	SpawnRotation.Pitch += UKismetMathLibrary::RandomFloatInRange(0, Spread.Y);
	return FTransform(SpawnRotation, SpawnLocation);
}
