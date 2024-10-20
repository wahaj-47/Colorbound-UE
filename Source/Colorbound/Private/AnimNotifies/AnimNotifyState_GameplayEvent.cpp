// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotifyState_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "PaperZDAnimInstance.h"

void UAnimNotifyState_GameplayEvent::OnNotifyBegin_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	// OwningInstance can be null on editor
	if (OwningInstance)
	{
		if (AActor* Owner = OwningInstance->GetOwningActor())
		{
			FGameplayEventData EventData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag_Begin, EventData);
		}
	}
}

void UAnimNotifyState_GameplayEvent::OnNotifyEnd_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	// OwningInstance can be null on editor
	if (OwningInstance)
	{
		if (AActor* Owner = OwningInstance->GetOwningActor())
		{
			FGameplayEventData EventData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag_End, EventData);
		}
	}
}
