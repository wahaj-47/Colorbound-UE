// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PaperZDAnimInstance.h"
#include "PlayAnimationAndWaitForEvent.generated.h"

class UPaperZDAnimSequence;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayAnimationAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/**
 * 
 */
UCLASS()
class COLORBOUND_API UPlayAnimationAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPlayAnimationAndWaitForEvent(const FObjectInitializer& ObjectInitializer);

	/** The animation completely finished playing */
	UPROPERTY(BlueprintAssignable)
	FPlayAnimationAndWaitForEventDelegate OnCompleted;

	/** The animation was interrupted */
	UPROPERTY(BlueprintAssignable)
	FPlayAnimationAndWaitForEventDelegate OnInterrupted;

	/** The ability task was explicitly cancelled by another ability */
	UPROPERTY(BlueprintAssignable)
	FPlayAnimationAndWaitForEventDelegate OnCancelled;

	/** One of the triggering gameplay events happened */
	UPROPERTY(BlueprintAssignable)
	FPlayAnimationAndWaitForEventDelegate EventReceived;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UPlayAnimationAndWaitForEvent* PlayAnimationAndWaitForEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		FGameplayTagContainer EventTags,
		UPaperZDAnimSequence* Animation,
		FName SlotName = "DefaultSlot",
		float StartingPosition = 0.f,
		float Rate = 1.f,
		bool bStopWhenAbilityEnds = true);

protected:
    // Begin UGameplayTask interface
    virtual void Activate() override;
	virtual void ExternalCancel() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;
    // End UGameplayTask interface
	
private:
	/** Animation that is playing */
	UPROPERTY()
	UPaperZDAnimSequence* Animation;

	UPROPERTY()
	FName SlotName;

	UPROPERTY()
	float StartingPosition;

	/** List of tags to match against gameplay events */
	UPROPERTY()
	FGameplayTagContainer EventTags;

	/** Playback rate */
	UPROPERTY()
	float Rate;

	/** Rather montage should be aborted if ability ends */
	UPROPERTY()
	bool bStopWhenAbilityEnds;

	void OnAbilityCancelled();
	void OnAnimationEnd(bool bCompleted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	bool StartPlayingAnimation();
	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	bool StopPlayingAnimation();

	FZDOnAnimationOverrideEndSignature AnimationEndDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
};
