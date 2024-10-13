// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Notifies/PaperZDAnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_GameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UAnimNotify_GameplayEvent : public UPaperZDAnimNotify
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category="Colorbound|Ability System")
	FGameplayTag EventTag;

	void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance = nullptr) const override;
	
};
