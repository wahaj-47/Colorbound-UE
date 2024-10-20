// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Notifies/PaperZDAnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_GameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UAnimNotifyState_GameplayEvent : public UPaperZDAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Colorbound|Ability System")
	FGameplayTag EventTag_Begin;

	UPROPERTY(EditAnywhere, Category = "Colorbound|Ability System")
	FGameplayTag EventTag_End;

	void OnNotifyBegin_Implementation(UPaperZDAnimInstance* OwningInstance) const override;
	void OnNotifyEnd_Implementation(UPaperZDAnimInstance* OwningInstance) const override;
};
