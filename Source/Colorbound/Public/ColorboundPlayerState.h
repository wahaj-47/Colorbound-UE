// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "ColorboundPlayerState.generated.h"

class UColorboundAbilitySystemComponent;
class UColorboundAttributeSet;

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AColorboundPlayerState();

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UColorboundAttributeSet* GetAttributeSet() const;
	
protected:

	UPROPERTY()
	TObjectPtr<UColorboundAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UColorboundAttributeSet> AttributeSet;

};
