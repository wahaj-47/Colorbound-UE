// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PaperZDAnimInstance.h"
#include "AbilitySystem/Abilities/ColorboundAbilityTypes.h"
#include "AbilitySystem/Abilities/GameplayAbilityRepAnimSequence.h"
#include "ColorboundAbilitySystemComponent.generated.h"

class UPaperZDAnimSequence;
struct FColorboundGameplayAbilityActorInfo;
struct FGameplayAbilityRepAnimSequence;

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	TSharedPtr<FColorboundGameplayAbilityActorInfo>	ColorboundAbilityActorInfo;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	UColorboundAbilitySystemComponent();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	// ----------------------------------------------------------------------------------------------------------------
	//	PaperZD AnimSequence Support
	// ----------------------------------------------------------------------------------------------------------------

	/** Plays a Sequence and handles replication and prediction based on passed in ability/activation info */
	float PlaySequence(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UPaperZDAnimSequence* Sequence, FName SlotName = "DefaultSlot", float InPlayRate = 1.0f, float StartingPosition = 0.0f);

	/** Plays a Sequence without updating replication/prediction structures. Used by simulated proxies when replication tells them to play a Sequence. */
	float PlaySequenceSimulated(UPaperZDAnimSequence* Sequence, FName SlotName = "DefaultSlot", float InPlayRate = 1.0f, float StartingPosition = 0.0f);

	/** Stops whatever Sequence is currently playing. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check) */
	void CurrentSequenceStop();

	/** Stops current Sequence if it's the one given as the Sequence param */
	void StopSequenceIfCurrent(const UPaperZDAnimSequence& Sequence);

	/** Clear the animating ability that is passed in, if it's still currently animating */
	virtual void ClearAnimatingAbility(UGameplayAbility* Ability) override;

	/** Returns true if the passed in ability is the current animating ability */
	bool IsAnimatingColorboundAbility(UGameplayAbility* Ability) const;

	/** Returns the current animating ability */
	UGameplayAbility* GetAnimatingColorboundAbility();

	/** Returns Sequence that is currently playing */
	UPaperZDAnimSequence* GetCurrentSequence() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	/** Set if sequence rep happens while we don't have the animinstance associated with us yet */
	UPROPERTY()
	bool bPendingSequenceRep;

	UPROPERTY()
	FGameplayAbilityLocalAnimSequence LocalAnimSequenceInfo;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAnimSequence)
	FGameplayAbilityRepAnimSequence RepAnimSequenceInfo;

	void SetRepAnimSequenceInfo(const FGameplayAbilityRepAnimSequence& NewRepAnimSequenceInfo);
	FGameplayAbilityRepAnimSequence& GetRepAnimSequenceInfo_Mutable();
	const FGameplayAbilityRepAnimSequence& GetRepAnimSequenceInfo() const;

	UFUNCTION()
	void OnRep_ReplicatedAnimSequence();

	/** Returns true if we are ready to handle replicated sequence information */
	virtual bool IsReadyForReplicatedSequence();

	/** Called when a prediction key that played a sequence is rejected */
	void OnPredictiveSequenceRejected(UPaperZDAnimSequence* PredictiveSequence);

	/** Copy LocalAnimSequenceInfo into RepAnimSequenceInfo */
	void AnimSequence_UpdateReplicatedData();
	void AnimSequence_UpdateReplicatedData(FGameplayAbilityRepAnimSequence& OutRepAnimSeqeunceInfo);
};
