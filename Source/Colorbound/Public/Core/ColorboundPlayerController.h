// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ColorboundPlayerController.generated.h"

class AColorboundPlayerState;
class UColorboundAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Colorbound|PlayerController")
	AColorboundPlayerState* GetColorboundPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|PlayerController")
	UColorboundAbilitySystemComponent* GetColorboundAbilitySystemComponent() const;

protected:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
};
