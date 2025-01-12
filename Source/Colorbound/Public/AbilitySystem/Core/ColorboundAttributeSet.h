// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ColorboundAttributeSet.generated.h"

/**
 * 
 */

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class COLORBOUND_API UColorboundAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UColorboundAttributeSet();

	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Health);

	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Strength);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Intelligence);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Resilience);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Vigor);

	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, ArmorPenetration);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, BlockChance);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, CriticalHitChance);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, CriticalHitDamage);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, CriticalHitResistance);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, HealthRegenRate)
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, MaxHealth)

	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, DamageResistance);
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Damage);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;

	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;

	UFUNCTION()
	void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_DamageResistance(const FGameplayAttributeData& OldDamageResistance) const;

private:
	// --------------------------------------------------------------------
	//	Vital Attribute
	// --------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Colorbound|Attributes|Vital", Meta=(AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// --------------------------------------------------------------------
	//	Primary Attributes
	// --------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Colorbound|Attributes|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Strength;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Colorbound|Attributes|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Intelligence;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Colorbound|Attributes|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Resilience;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Colorbound|Attributes|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Vigor;

	// --------------------------------------------------------------------
	//	Secondary Attributes
	// --------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ArmorPenetration;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BlockChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalHitChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalHitDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalHitResistance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegenRate, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthRegenRate;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// --------------------------------------------------------------------
	//	Resistance Attribute
	// --------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageResistance, Category = "Colorbound|Attributes|Secondary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageResistance;

	// --------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// --------------------------------------------------------------------

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category = "Colorbound|Meta Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

};
