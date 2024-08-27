// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorboundCharacterBase.h"
#include "GameplayTagContainer.h"
#include "ColorboundPlayerCharacter.generated.h"

class UColorboundAbilitySet;
class UColorboundInputConfig;

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundPlayerCharacter : public AColorboundCharacterBase
{
	GENERATED_BODY()

private:
	void InitAbilitySystemComponent();

protected:
	void InputAbilityInputTagPressed(FGameplayTag InputTag);
	void InputAbilityInputTagReleased(FGameplayTag InputTag);

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputSystem|Input")
	UColorboundInputConfig* InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputSystem|Input")
	class UInputMappingContext* DefaultMappingContext;

};
