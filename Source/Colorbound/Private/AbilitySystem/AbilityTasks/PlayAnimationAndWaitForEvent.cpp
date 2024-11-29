// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/PlayAnimationAndWaitForEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Core/ColorboundGameplayAbility.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"

UPlayAnimationAndWaitForEvent::UPlayAnimationAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SlotName = "DefaultSlot";
	StartingPosition = 0.f;
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UPlayAnimationAndWaitForEvent* UPlayAnimationAndWaitForEvent::PlayAnimationAndWaitForEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, FGameplayTagContainer EventTags, UPaperZDAnimSequence* Animation, FName SlotName, float StartingPosition, float Rate, bool bStopWhenAbilityEnds)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UPlayAnimationAndWaitForEvent* MyObj = NewAbilityTask<UPlayAnimationAndWaitForEvent>(OwningAbility, TaskInstanceName);
	MyObj->EventTags = EventTags;
	MyObj->Animation = Animation;
	MyObj->SlotName = SlotName;
	MyObj->StartingPosition = StartingPosition;
	MyObj->Rate = Rate;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return MyObj;
}

void UPlayAnimationAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedAnimation = false;
	
	if(UColorboundAbilitySystemComponent* ASC = Cast<UColorboundAbilitySystemComponent>(AbilitySystemComponent.Get()))
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UPaperZDAnimInstance* AnimInstance = Cast<UColorboundGameplayAbility>(Ability)->GetColorboundActorInfo(ActorInfo)->GetAnimInstance();
		if (AnimInstance)
		{
			EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnGameplayEvent));
			AnimationEndDelegate.BindUObject(this, &ThisClass::OnAnimationEnd);

			if (ASC->PlaySequence(Ability, Ability->GetCurrentActivationInfo(), Animation, SlotName, Rate, StartingPosition) > 0.0f)
			{
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &ThisClass::OnAbilityCancelled);
				bPlayedAnimation = true;
			}
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAbilityTask_PlayAnimationAndWaitForEvent call to PlayAnimationOverride failed!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UAbilityTask_PlayAnimationAndWaitForEvent called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedAnimation)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAbilityTask_PlayAnimationAndWaitForEvent called in Ability %s failed to play animation %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(Animation), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UPlayAnimationAndWaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent.IsValid());

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UPlayAnimationAndWaitForEvent::OnDestroy(bool bInOwnerFinished)
{
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (bInOwnerFinished && bStopWhenAbilityEnds)
		{
			StopPlayingAnimation();
		}
	}

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UPlayAnimationAndWaitForEvent::OnAbilityCancelled()
{
	if (StopPlayingAnimation())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UPlayAnimationAndWaitForEvent::OnAnimationEnd(bool bCompleted)
{
	if (bCompleted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void UPlayAnimationAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

bool UPlayAnimationAndWaitForEvent::StartPlayingAnimation()
{
	return false;
}

bool UPlayAnimationAndWaitForEvent::StopPlayingAnimation()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	UPaperZDAnimInstance* AnimInstance = Cast<UColorboundGameplayAbility>(Ability)->GetColorboundActorInfo(ActorInfo)->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	if (Ability && AbilitySystemComponent.IsValid())
	{
		AnimationEndDelegate.ExecuteIfBound(false);
		return true;
	}

	return false;
}
