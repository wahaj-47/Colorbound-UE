// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingSelectionTask_AOE.h"
#include "Colorbound_TSTask_AOE.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorbound_TSTask_AOE : public UTargetingSelectionTask_AOE
{
	GENERATED_BODY()

protected:

	FVector GetSourceOffset_Implementation(const FTargetingRequestHandle& TargetingHandle) const override;

};
