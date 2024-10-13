// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "PaperZDAnimInstance.h"

void UAnimNotify_GameplayEvent::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	// OwningInstance can be null on editor
	if (OwningInstance)
	{
		if (AActor* Owner = OwningInstance->GetOwningActor()) 
		{
			FGameplayEventData EventData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, EventData);
		}
	}
}
