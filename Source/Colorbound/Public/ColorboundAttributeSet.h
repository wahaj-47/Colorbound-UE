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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category="Attributes|Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Health);

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributues|Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, MaxHealth)

	// Health regen rate will passively increase Health every second
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, HealthRegenRate)

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes|Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, Damage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageResistance,  Category = "Meta Attributes|Damage")
	FGameplayAttributeData DamageResistance;
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, DamageResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CharacterLevel, Category = "Attributes|Character Level")
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UColorboundAttributeSet, CharacterLevel)

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	
	UFUNCTION()
	void OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel) const;

	UFUNCTION()
	void OnRep_DamageResistance(const FGameplayAttributeData& OldDamageResistance) const;
};
