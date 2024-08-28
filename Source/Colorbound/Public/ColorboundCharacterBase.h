// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "AbilitySystemInterface.h"
#include "ColorboundCharacterBase.generated.h"

class UGameplayAbility;
class UColorboundAbilitySystemComponent;
class UColorboundAttributeSet;
class UGameplayEffect;
class UColorboundAbilitySet;

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundCharacterBase : public APaperZDCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UColorboundAttributeSet* GetAttributeSet() const;
	/**
	* Getters for attributes from GDAttributeSetBase
	**/
	UFUNCTION(BlueprintCallable, Category = "Colorbound|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Axis")
	FVector2D GetForwardVector() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Axis")
	FVector2D GetRightVector() const;
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Colorbound|Axis")
	FVector2D ForwardVector = FVector2D(1,0);

	UPROPERTY(BlueprintReadOnly, Category = "Colorbound|Axis")
	FVector2D RightVector = FVector2D(0,-1);

	UPROPERTY()
	TObjectPtr<UColorboundAbilitySystemComponent> AbilitySystemComponent;

	TObjectPtr<UColorboundAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputSystem|Abilities")
	UColorboundAbilitySet* AbilitySet;

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAbilitySet();
};
