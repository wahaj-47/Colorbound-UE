// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/ColorboundCharacterBase.h"
#include "GameplayTagContainer.h"
#include "ColorboundPlayerCharacter.generated.h"

class UInputMappingContext;
class UColorboundAbilitySet;
class UColorboundInputConfig;
class UAbilityQueueComponent;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundPlayerCharacter : public AColorboundCharacterBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability System", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilityQueueComponent> AbilityQueueComponent;

	UFUNCTION()
	void InitAbilitySystemComponent();

protected:
	virtual void BeginPlay() override;
	void InputAbilityInputTagPressed(FGameplayTag InputTag);
	void InputAbilityInputTagReleased(FGameplayTag InputTag);

public:
	AColorboundPlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;


	// Character Stats Interface
	virtual int32 GetCharacterLevel_Implementation() const override;
	virtual int32 GetCharacterXP_Implementation() const override;
	// end Character Stats Interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputSystem|Input")
	UColorboundInputConfig* InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputSystem|Input")
	UInputMappingContext* DefaultMappingContext;

	UFUNCTION()
	void Input_Move(const FInputActionValue& InputActionValue);

};
