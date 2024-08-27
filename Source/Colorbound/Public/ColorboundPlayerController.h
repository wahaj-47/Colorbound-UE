// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ColorboundPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
};
