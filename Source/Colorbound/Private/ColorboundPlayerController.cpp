// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundPlayerController.h"
#include "ColorboundPlayerState.h"
#include "ColorboundAbilitySystemComponent.h"

void AColorboundPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	AColorboundPlayerState* PS = CastChecked<AColorboundPlayerState>(PlayerState, ECastCheckedType::NullAllowed);

	if (UColorboundAbilitySystemComponent* ASC = CastChecked<UColorboundAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
