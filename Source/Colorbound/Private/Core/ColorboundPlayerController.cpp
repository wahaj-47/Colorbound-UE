// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ColorboundPlayerController.h"
#include "Core/ColorboundPlayerState.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"

AColorboundPlayerState* AColorboundPlayerController::GetColorboundPlayerState() const
{
	return CastChecked<AColorboundPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UColorboundAbilitySystemComponent* AColorboundPlayerController::GetColorboundAbilitySystemComponent() const
{
	const AColorboundPlayerState* ColorboundPS = GetColorboundPlayerState();
	return (ColorboundPS ? ColorboundPS->GetColorboundAbilitySystemComponent() : nullptr);
}

void AColorboundPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UColorboundAbilitySystemComponent* ColorboundASC = GetColorboundAbilitySystemComponent())
	{
		ColorboundASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
