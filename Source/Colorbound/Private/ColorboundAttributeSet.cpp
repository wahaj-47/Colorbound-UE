// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "ColorboundCharacterBase.h"

UColorboundAttributeSet::UColorboundAttributeSet()
{
}

void UColorboundAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, DamageResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);

}

void UColorboundAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
}

void UColorboundAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	AActor* TargetActor = nullptr;
	AColorboundCharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetCharacter = Cast<AColorboundCharacterBase>(TargetActor);
	}
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		float InDamage = GetDamage();

		SetDamage(0.0f);

		if (InDamage > 0.0f)
		{
			const float NewHealth = GetHealth() - InDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			bool bIsAlive = GetHealth() > 0.0f;

			if (bIsAlive)
			{
				TargetCharacter->HitReact();
			}
		}
	}
}

void UColorboundAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, Health, OldHealth);
}

void UColorboundAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, MaxHealth, OldMaxHealth);
}

void UColorboundAttributeSet::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, CharacterLevel, OldCharacterLevel);
}

void UColorboundAttributeSet::OnRep_DamageResistance(const FGameplayAttributeData& OldDamageResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, DamageResistance, OldDamageResistance);
}
