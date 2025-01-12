// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ColorboundPlayerController.generated.h"

class AColorboundPlayerState;
class UColorboundAbilitySystemComponent;
class UDamageTextWidgetComponent;

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

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, AActor* TargetActor, bool bBlockedHit, bool bCriticalHit);

protected:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	UPROPERTY(EditAnywhere, Category = "Colorbound|UI")
	TSubclassOf<UDamageTextWidgetComponent> DamageTextComponentClass;
	
};
