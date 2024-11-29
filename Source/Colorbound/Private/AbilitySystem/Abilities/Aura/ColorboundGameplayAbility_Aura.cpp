// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Aura/ColorboundGameplayAbility_Aura.h"
#include "Components/SphereComponent.h"
#include "Characters/ColorboundPlayerCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UColorboundGameplayAbility_Aura::UColorboundGameplayAbility_Aura(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UColorboundGameplayAbility_Aura::AddAuraComponent()
{
    AActor* Source = GetAvatarActorFromActorInfo();
    if (Source)
    {
        if (!AuraComponent)
        {
            AuraComponent = Cast<USphereComponent>(Source->AddComponentByClass(USphereComponent::StaticClass(), false, FTransform::Identity, false));
            
            AuraComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
            AuraComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
            AuraComponent->SetSphereRadius(AuraRadius);
        }
    }
}
