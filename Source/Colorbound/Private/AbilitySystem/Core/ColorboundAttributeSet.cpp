// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/ColorboundAttributeSet.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemLibrary.h"
#include "Core/ColorboundCharacterBase.h"
#include "Core/ColorboundPlayerController.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "ColorboundGameplayTags.h"

UColorboundAttributeSet::UColorboundAttributeSet()
{
}

void UColorboundAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, Health, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UColorboundAttributeSet, DamageResistance, COND_None, REPNOTIFY_Always);
}

void UColorboundAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UColorboundAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AColorboundCharacterBase* TargetCharacter = nullptr;
	UAbilitySystemComponent* TargetASC = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AColorboundCharacterBase>(TargetActor);
		TargetASC = Data.Target.AbilityActorInfo->AbilitySystemComponent.Get();
	}

	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AColorboundCharacterBase* SourceCharacter = nullptr;
	UAbilitySystemComponent* SourceASC = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<AColorboundCharacterBase>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<AColorboundCharacterBase>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}

		SourceASC = Source->AbilityActorInfo->AbilitySystemComponent.Get();
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
			const bool bBlockedHit = UColorboundAbilitySystemLibrary::EffectContextIsBlockedHit(Context);
			const bool bCriticalHit = UColorboundAbilitySystemLibrary::EffectContextIsCriticalHit(Context);

			if (bIsAlive)
			{
				FGameplayTagContainer TagContainer;
				if (bCriticalHit)
				{
					TagContainer.AddTag(ColorboundGameplayTags::AbilityTag_Knockback);
					TargetASC->TryActivateAbilitiesByTag(TagContainer);
				}
				else
				{
					TagContainer.AddTag(ColorboundGameplayTags::AbilityTag_HitReact);
					TargetASC->TryActivateAbilitiesByTag(TagContainer);
				}

				if (SourceActor != TargetActor)
				{
					AColorboundPlayerController* PC = Cast<AColorboundPlayerController>(SourceController);
					if (PC)
					{
						PC->ShowDamageNumber(InDamage, TargetCharacter, bBlockedHit, bCriticalHit);
					}
				}
			}

		}
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
}

void UColorboundAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, Health, OldHealth);
}

void UColorboundAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, Strength, OldStrength);
}

void UColorboundAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, Intelligence, OldIntelligence);
}

void UColorboundAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, Resilience, OldResilience);
}

void UColorboundAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, Vigor, OldVigor);
}

void UColorboundAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, Armor, OldArmor);
}

void UColorboundAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UColorboundAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, BlockChance, OldBlockChance);
}

void UColorboundAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UColorboundAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UColorboundAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UColorboundAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UColorboundAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, MaxHealth, OldMaxHealth);
}

void UColorboundAttributeSet::OnRep_DamageResistance(const FGameplayAttributeData& OldDamageResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UColorboundAttributeSet, DamageResistance, OldDamageResistance);
}
