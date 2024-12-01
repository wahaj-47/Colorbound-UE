// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "AbilitySystem/Core/ColorboundGameplayAbility.h"
#include "AnimSequences/Players/PaperZDAnimPlayer.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"

void UColorboundAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	ColorboundAbilityActorInfo = StaticCastSharedPtr<FColorboundGameplayAbilityActorInfo>(AbilityActorInfo);
}

UColorboundAbilitySystemComponent::UColorboundAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	bPendingSequenceRep = true;

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UColorboundAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UColorboundAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UColorboundAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UColorboundGameplayAbility* Ability = CastChecked<UColorboundGameplayAbility>(AbilitySpec->Ability);

				if (Ability->GetActivationPolicy() == EColorboundAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UColorboundGameplayAbility* Ability = CastChecked<UColorboundGameplayAbility>(AbilitySpec->Ability);

					if (Ability->GetActivationPolicy() == EColorboundAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UColorboundAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UColorboundAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(UColorboundAbilitySystemComponent, RepAnimSequenceInfo, Params);

}

void UColorboundAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UColorboundAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

float UColorboundAbilitySystemComponent::PlaySequence(UGameplayAbility* InAnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UPaperZDAnimSequence* NewAnimSequence, FZDOnAnimationOverrideEndSignature& OnOverrideEnd, FName SlotName, float InPlayRate, float StartingPosition)
{
	float Duration = -1.f;
	UPaperZDAnimInstance* AnimInstance = ColorboundAbilityActorInfo.IsValid() ? ColorboundAbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance && NewAnimSequence)
	{
		Duration = NewAnimSequence->GetTotalDuration();
		AnimInstance->PlayAnimationOverride(NewAnimSequence, SlotName, InPlayRate, StartingPosition, OnOverrideEnd);

		if (Duration > 0.f)
		{
			UPaperZDAnimSequence* Animation = NewAnimSequence;

			LocalAnimSequenceInfo.AnimSequence = NewAnimSequence;
			LocalAnimSequenceInfo.AnimatingAbility = InAnimatingAbility;
			LocalAnimSequenceInfo.PlayInstanceId = (LocalAnimSequenceInfo.PlayInstanceId < UINT8_MAX ? LocalAnimSequenceInfo.PlayInstanceId + 1 : 0);

			if (InAnimatingAbility)
			{
				Cast<UColorboundGameplayAbility>(InAnimatingAbility)->SetCurrentSequence(NewAnimSequence);
			}

			// Replicate for non-owners and for replay recordings
			// The data we set from GetRepAnimMontageInfo_Mutable() is used both by the server to replicate to clients and by clients to record replays.
			// We need to set this data for recording clients because there exists network configurations where an abilities montage data will not replicate to some clients (for example: if the client is an autonomous proxy.)
			if (ShouldRecordMontageReplication())
			{
				FGameplayAbilityRepAnimSequence& MutableRepAnimSequenceInfo = GetRepAnimSequenceInfo_Mutable();

				// Those are static parameters, they are only set when the sequence is played. They are not changed after that.
				MutableRepAnimSequenceInfo.Animation = Animation;
				MutableRepAnimSequenceInfo.PlayInstanceId = (MutableRepAnimSequenceInfo.PlayInstanceId < UINT8_MAX ? MutableRepAnimSequenceInfo.PlayInstanceId + 1 : 0);
				MutableRepAnimSequenceInfo.SlotName = SlotName;
				MutableRepAnimSequenceInfo.PlayRate = InPlayRate;
				MutableRepAnimSequenceInfo.TriggeredTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

				// Update parameters that change during sequence life time.
				AnimSequence_UpdateReplicatedData();
			}

			// Replicate to non-owners
			if (IsOwnerActorAuthoritative())
			{
				// Force net update on our avatar actor.
				if (AbilityActorInfo->AvatarActor != nullptr)
				{
					AbilityActorInfo->AvatarActor->ForceNetUpdate();
				}
			}
			else
			{
				// If this prediction key is rejected, we need to end the preview
				FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
				if (PredictionKey.IsValidKey())
				{
					PredictionKey.NewRejectedDelegate().BindUObject(this, &UColorboundAbilitySystemComponent::OnPredictiveSequenceRejected, NewAnimSequence);
				}
			}
		}
	}

	return Duration;
}

float UColorboundAbilitySystemComponent::PlaySequenceSimulated(UPaperZDAnimSequence* NewAnimSequence, FName SlotName, float InPlayRate, float StartingPosition)
{
	float Duration = -1.f;
	UPaperZDAnimInstance* AnimInstance = ColorboundAbilityActorInfo.IsValid() ? ColorboundAbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance && NewAnimSequence)
	{
		Duration = NewAnimSequence->GetTotalDuration();
		bool bQueued = AnimInstance->PlayAnimationOverride(NewAnimSequence, SlotName, InPlayRate, StartingPosition);

		if (Duration > 0.f && bQueued)
		{
			LocalAnimSequenceInfo.AnimSequence = NewAnimSequence;
		}
	}

	return Duration;
}

void UColorboundAbilitySystemComponent::CurrentSequenceStop()
{
	UPaperZDAnimInstance* AnimInstance = ColorboundAbilityActorInfo.IsValid() ? ColorboundAbilityActorInfo->GetAnimInstance() : nullptr;
	UPaperZDAnimSequence* SequenceToStop = LocalAnimSequenceInfo.AnimSequence;
	bool bShouldStopMontage = AnimInstance && SequenceToStop && AnimInstance->GetPlayer()->GetCurrentAnimSequence() == SequenceToStop;

	if (bShouldStopMontage)
	{
		AnimInstance->StopAnimationOverrideByGroup();

		if (IsOwnerActorAuthoritative())
		{
			AnimSequence_UpdateReplicatedData();
		}
	}
}

void UColorboundAbilitySystemComponent::StopSequenceIfCurrent(const UPaperZDAnimSequence& Sequence)
{
	if (&Sequence == LocalAnimSequenceInfo.AnimSequence)
	{
		CurrentSequenceStop();
	}

}

void UColorboundAbilitySystemComponent::ClearAnimatingAbility(UGameplayAbility* Ability)
{
	if (LocalAnimSequenceInfo.AnimatingAbility.Get() == Ability)
	{
		Cast<UColorboundGameplayAbility>(Ability)->SetCurrentSequence(nullptr);
		LocalAnimSequenceInfo.AnimatingAbility = nullptr;
	}
}

bool UColorboundAbilitySystemComponent::IsAnimatingColorboundAbility(UGameplayAbility* InAbility) const
{
	return (LocalAnimSequenceInfo.AnimatingAbility.Get() == InAbility);
}

UGameplayAbility* UColorboundAbilitySystemComponent::GetAnimatingColorboundAbility()
{
	return LocalAnimSequenceInfo.AnimatingAbility.Get();
}

UPaperZDAnimSequence* UColorboundAbilitySystemComponent::GetCurrentSequence() const
{
	UPaperZDAnimInstance* AnimInstance = ColorboundAbilityActorInfo.IsValid() ? ColorboundAbilityActorInfo->GetAnimInstance() : nullptr;
	if (LocalAnimSequenceInfo.AnimSequence && AnimInstance && AnimInstance->GetPlayer()->GetCurrentAnimSequence() == LocalAnimSequenceInfo.AnimSequence)
	{
		return LocalAnimSequenceInfo.AnimSequence;
	}

	return nullptr;
}

void UColorboundAbilitySystemComponent::SetRepAnimSequenceInfo(const FGameplayAbilityRepAnimSequence& NewRepAnimSequenceInfo)
{
	GetRepAnimSequenceInfo_Mutable() = NewRepAnimSequenceInfo;
}

FGameplayAbilityRepAnimSequence& UColorboundAbilitySystemComponent::GetRepAnimSequenceInfo_Mutable()
{
	return RepAnimSequenceInfo;
}

const FGameplayAbilityRepAnimSequence& UColorboundAbilitySystemComponent::GetRepAnimSequenceInfo() const
{
	return RepAnimSequenceInfo;
}

void UColorboundAbilitySystemComponent::OnRep_ReplicatedAnimSequence()
{
	UWorld* World = GetWorld();

	const FGameplayAbilityRepAnimSequence& ConstRepAnimSequenceInfo = GetRepAnimSequenceInfo_Mutable();

	if (ConstRepAnimSequenceInfo.bSkipPlayRate)
	{
		GetRepAnimSequenceInfo_Mutable().PlayRate = 1.f;
	}

	const bool bIsPlayingReplay = World && World->IsPlayingReplay();

	const float SEQUENCE_REP_POS_ERR_THRESH = 0.1f;

	UPaperZDAnimInstance* AnimInstance = ColorboundAbilityActorInfo.IsValid() ? ColorboundAbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance == nullptr || !IsReadyForReplicatedSequence())
	{
		// We can't handle this yet
		bPendingSequenceRep = true;
		return;
	}
	bPendingSequenceRep = false;

	if (!AbilityActorInfo->IsLocallyControlled())
	{
		if (ConstRepAnimSequenceInfo.Animation)
		{
			// New Sequence to play
			UPaperZDAnimSequence* LocalAnimation = LocalAnimSequenceInfo.AnimSequence;
			if ((LocalAnimation != ConstRepAnimSequenceInfo.Animation) ||
				(LocalAnimSequenceInfo.PlayInstanceId != ConstRepAnimSequenceInfo.PlayInstanceId))
			{
				LocalAnimSequenceInfo.PlayInstanceId = ConstRepAnimSequenceInfo.PlayInstanceId;
				PlaySequenceSimulated(ConstRepAnimSequenceInfo.Animation, ConstRepAnimSequenceInfo.SlotName, ConstRepAnimSequenceInfo.PlayRate);
			}

			if (LocalAnimSequenceInfo.AnimSequence == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OnRep_ReplicatedAnimMontage: PlaySequenceSimulated failed. Name: %s, Animation: %s"), *GetNameSafe(this), *GetNameSafe(ConstRepAnimSequenceInfo.Animation));
				return;
			}

			// Compressed Flags
			const bool bIsStopped = AnimInstance->GetPlayer()->GetCurrentAnimSequence() == LocalAnimSequenceInfo.AnimSequence && !AnimInstance->GetPlayer()->IsPlaying();
			const bool bReplicatedIsStopped = bool(ConstRepAnimSequenceInfo.IsStopped);

			// Process stopping first, so we don't change sections and cause blending to pop.
			if (bReplicatedIsStopped)
			{
				if (!bIsStopped)
				{
					AnimInstance->StopAnimationOverrideByGroup();
				}
			}
			else
			{
				// Update Position. If error is too great, jump to replicated position.
				const float CurrentServerTime = World->GetGameState()->GetServerWorldTimeSeconds();
				const float DeltaServerTime = ConstRepAnimSequenceInfo.TriggeredTime - CurrentServerTime;
				const float NewPosition = FMath::Clamp(DeltaServerTime, 0, ConstRepAnimSequenceInfo.Animation->GetTotalDuration());

				if ((FMath::Abs(DeltaServerTime) > SEQUENCE_REP_POS_ERR_THRESH) && (ConstRepAnimSequenceInfo.IsStopped == 0))
				{
					// Skip triggering notifies if we're going backwards in time, we've already triggered them.
					const float DeltaTime = !FMath::IsNearlyZero(ConstRepAnimSequenceInfo.PlayRate) ? (NewPosition / ConstRepAnimSequenceInfo.PlayRate) : 0.f;
					if (DeltaTime >= 0.f)
					{
						AnimInstance->GetPlayer()->ProcessAnimSequenceNotifies(
							LocalAnimSequenceInfo.AnimSequence,
							DeltaTime,
							NewPosition,
							0
						);
					}
					// @TODO: Fast forward to server position
					PlaySequenceSimulated(ConstRepAnimSequenceInfo.Animation, ConstRepAnimSequenceInfo.SlotName, ConstRepAnimSequenceInfo.PlayRate, NewPosition);
				}
			}
		}
	}
}

bool UColorboundAbilitySystemComponent::IsReadyForReplicatedSequence()
{
	/** Children may want to override this for additional checks (e.g, "has skin been applied") */
	return true;
}

void UColorboundAbilitySystemComponent::OnPredictiveSequenceRejected(UPaperZDAnimSequence* PredictiveSequence)
{
	UPaperZDAnimInstance* AnimInstance = ColorboundAbilityActorInfo.IsValid() ? ColorboundAbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance && PredictiveSequence)
	{
		// If this sequence is still playing: kill it
		if (AnimInstance->GetPlayer()->GetCurrentAnimSequence() == PredictiveSequence)
		{
			// TODO: Figure out the seqeunce group and use it to stop playback 
			AnimInstance->StopAnimationOverrideByGroup();
		}
	}
}

void UColorboundAbilitySystemComponent::AnimSequence_UpdateReplicatedData()
{
	check(ShouldRecordMontageReplication());

	AnimSequence_UpdateReplicatedData(GetRepAnimSequenceInfo_Mutable());
}

void UColorboundAbilitySystemComponent::AnimSequence_UpdateReplicatedData(FGameplayAbilityRepAnimSequence& OutRepAnimSeqeunceInfo)
{
	const UPaperZDAnimInstance* AnimInstance = ColorboundAbilityActorInfo.IsValid() ? ColorboundAbilityActorInfo->GetAnimInstance() : nullptr;
	const UPaperZDAnimPlayer* AnimPlayer = AnimInstance ? AnimInstance->GetPlayer() : nullptr;
	if (AnimInstance && LocalAnimSequenceInfo.AnimSequence)
	{
		OutRepAnimSeqeunceInfo.Animation = LocalAnimSequenceInfo.AnimSequence;

		// Compressed Flags
		const bool bIsStopped = AnimPlayer->GetCurrentAnimSequence() == OutRepAnimSeqeunceInfo.Animation && !AnimPlayer->IsPlaying();		

		if (OutRepAnimSeqeunceInfo.IsStopped != bIsStopped)
		{
			// Set this prior to calling UpdateShouldTick, so we start ticking if we are playing a Montage
			OutRepAnimSeqeunceInfo.IsStopped = bIsStopped;

			// When we start or stop an animation, update the clients right away for the Avatar Actor
			if (AbilityActorInfo->AvatarActor != nullptr)
			{
				AbilityActorInfo->AvatarActor->ForceNetUpdate();
			}

			// When this changes, we should update whether or not we should be ticking
			UpdateShouldTick();
		}
	}
}
