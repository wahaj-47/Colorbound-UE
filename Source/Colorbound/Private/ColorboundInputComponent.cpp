// Fill out your copyright notice in the Description page of Project Settings.

#include "ColorboundInputComponent.h"

#include "EnhancedInputSubsystems.h"

UColorboundInputComponent::UColorboundInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UColorboundInputComponent::AddInputMappings(const UColorboundInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UColorboundInputComponent::RemoveInputMappings(const UColorboundInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UColorboundInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
