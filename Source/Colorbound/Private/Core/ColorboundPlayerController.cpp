// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ColorboundPlayerController.h"
#include "Core/ColorboundPlayerState.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "UI/DamageTextWidgetComponent.h"
#include "Subsystems/ObjectPoolWorldSubsystem.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

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

void AColorboundPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AActor* TargetActor, bool bBlockedHit, bool bCriticalHit)
{
	UObjectPoolWorldSubsystem* ObjectPool = GetPawn()->GetWorld()->GetSubsystem<UObjectPoolWorldSubsystem>();
	
	UDamageTextWidgetComponent* DamageText = Cast<UDamageTextWidgetComponent>(ObjectPool->AddPooledComponentByClass(TargetActor, DamageTextComponentClass, false, FTransform::Identity, true));
	ObjectPool->FinishAddPooledComponent(TargetActor, DamageText, false, FTransform::Identity);

	DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
}
